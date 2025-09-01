#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\cCreatureGameData.h>
#include "Common.h"
// EP1 Ingame Behaviors
#include "EP1_PosseCommand.h"
#include "EP1_CaptainAbilities.h"
#include "EP1_GameplayObjectManager.h"
#include "EP1_GameplayObject_HoloProjector.h"
#include "EP1_GameplayObject_DriveMarker.h"
#include "EP1_GameplayObject_IceCube.h"
#include "EP1_BehaviorManager.h"
// Scripts
#include "CR_JetHover.h"

#define HBEp1Ptr intrusive_ptr<HBEp1>
using namespace Simulator;

// EP1

EP1_PosseCommand* ep1_possecommand;
EP1_CaptainAbilities* ep1_captainabilities;
// manager
EP1_GameplayObjectManager* gameplayobjectmanager;
// submanagers
EP1_GameplayObject_HoloProjector* ep1_gameplayobject_projector;
EP1_GameplayObject_DriveMarker* ep1_gameplayobject_drivemarker;
EP1_GameplayObject_IceCube* ep1_gameplayobject_icecube;
EP1_BehaviorManager* ep1_behaviormanager;

class HBEp1
{

public:

	static void Initialize() {
		// EP1
		ep1_possecommand = new(EP1_PosseCommand);
		ep1_captainabilities = new(EP1_CaptainAbilities);

		// Initialize Manager
		gameplayobjectmanager = new(EP1_GameplayObjectManager);
		// Initialize Submanagers
		ep1_gameplayobject_projector = new(EP1_GameplayObject_HoloProjector);
		ep1_gameplayobject_icecube = new(EP1_GameplayObject_IceCube);
		// Add to manager
		gameplayobjectmanager->AddGameplayObjectSubmanager(ep1_gameplayobject_projector);
		gameplayobjectmanager->AddGameplayObjectSubmanager(ep1_gameplayobject_icecube);

		ep1_gameplayobject_drivemarker = new(EP1_GameplayObject_DriveMarker);

		ep1_behaviormanager = new(EP1_BehaviorManager);
	}

	static void Dispose() {
		ep1_possecommand = nullptr;
		ep1_captainabilities = nullptr;
		gameplayobjectmanager = nullptr;
		ep1_gameplayobject_projector = nullptr;
		ep1_gameplayobject_drivemarker = nullptr;
		ep1_gameplayobject_icecube = nullptr;
		ep1_behaviormanager = nullptr;
	}

	static void AttachDetours();
};

//----------
// Detours

// Spui loading/spawning detour
member_detour(EP1_ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey & name, bool arg_4, uint32_t arg_8) {

		if (IsScenarioMode()) {
			if (name.instanceID == 0x5B3E66CC) { //id("ScenarioEditModeBehaviorChatter")
				bool ScenarioSpuiSpawned = original_function(this, name, arg_4, arg_8);
				if (ScenarioSpuiSpawned) {
					// spawn the vehicle specific spui to add to this
					// TODO: use a new class to handle this code
					ep1_behaviormanager->ApplyVehicleBehaviorUIs();

				}
				// do this last
				return ScenarioSpuiSpawned;
			}
		}
		//else if (IsCivGame()) {
		//	if (name.instanceID == id("Rollover_CivVehicle")) {
		//
		//	}
		//}

		// planner popup
		//if (name.instanceID == id("plannerPalette")) {
		//	MessageManager.MessageSend(id("PlannerPopup"), nullptr);
		//}


		return original_function(this, name, arg_4, arg_8);
	}
};

// For keeping gameplay objects in sync with scenario goals
// TODO: move to gameplayobjectmanager?
member_detour(ScenarioPlayModeUpdateGoals_detour, Simulator::cScenarioPlayMode, bool())
{
	bool detoured()
	{
		bool result = original_function(this);
		gameplayobjectmanager->PropagateAction(EP1_GameplayObjectManager::UpdateScenarioGoals);
		return result;
	}
};

// Detour the playanimation ID-picking func
static bool EP1_AnimOverride_detour(Anim::AnimatedCreature* obj, uint32_t& animID, int* pChoice) {

	if (IsCreatureGame() || IsScenarioMode()) {

		// replace the jump jet animations when in mid-flight
		uint32_t jetanim = JetHover_GetAnim(animID);
		if (jetanim != animID && ep1_captainabilities->can_jumpburst) {
			animID = jetanim; return true;
		}

		// Scenario Social behaviors
		// detect when creature waves after being allied
		if (animID == 0xb225328f) { // "soc_hurrayclap"
			auto creature = Common::GetAnimCreatureOwner(obj);
			// if it is a sentient creature, replace the clapping with a salute
			if (creature && creature->GetCurrentBrainLevel() >= 5 && creature->mAge >= 1) {
				animID = 0x00BF10B3; return true; //"soc_posse_salute_01a"
			}

		}

		// make holograms not play the hover animation
		if (animID == 0x0692E6A9) { // "gen_jump_loop"
			auto creature = Common::GetAnimCreatureOwner(obj);
			if (creature && ep1_gameplayobject_projector->IsCreatureHologram(creature)) {
				animID = 0x04330667; return true; //"csa_idle_gen_editor"
			}

		}

		// Swap the wave for a flex intimidate.
		if (animID == 0x5403B863) { // "gen_posse_call"
			// Make this only play if avatar in combat stance mode when interacting
			if (CreatureGameData.GetAbilityMode() == cCreatureGameData::AbilityMode::Attack) {
				animID = 0x0418B841; return true; //"csa_phpto_flex" (sic) 
			}
		}
	}

	return false;
}

// Detour Creature WalkTo
virtual_detour(EP1_WalkTo_detour, Simulator::cCreatureAnimal, Simulator::cCreatureBase, void(int, const Vector3&, const Vector3&, float, float)) //cCreatureAnimal
{
	void detoured(int speedState, const Vector3 & dstPos, const Vector3 & arg_8, float goalStopDistance = 1.0f, float acceptableStopDistance = 2.0f)
	{
		/*
		for (auto creature : ep1_gameplayobject_projector->mHolograms) {
			if (object_cast<Simulator::cCreatureAnimal>(this) == creature) {
				return;
			}
		}*/
		// Check if the creature is in the captain posse playing a scheduled walk. If so, do not interrupt them!
		auto schedule = ep1_possecommand->GetCreatureSchedule(object_cast<Simulator::cCreatureAnimal>(this));

		auto pos = GameViewManager.GetWorldMousePosition(0);
		if (schedule && (!schedule->mbCanMove && pos != dstPos)) {
			return;
		}
		original_function(this, speedState, dstPos, arg_8, goalStopDistance, acceptableStopDistance);
	}
};

// Detour Creature OnJumpLand
virtual_detour(EP1_OnJumpLand_detour, Simulator::cCreatureAnimal, Simulator::cCreatureBase, void())
{
	void detoured()
	{
		ep1_captainabilities->ResetJumpCount();
		original_function(this);
	}
};

// Called when a combatant takes damage
virtual_detour(EP1_CombatTakeDamage_detour, Simulator::cCombatant, Simulator::cCombatant, int(float, uint32_t, int, const Vector3&, cCombatant*))
{
	int detoured(float damage, uint32_t attackerPoliticalID, int integer, const Vector3 & vector, cCombatant * pAttacker)
	{
		// send the message of this to the gameplayobjectmanager, including the attacker and object
		// and have that class propogate it to the rest of the submanagers to check for if the object can be handled, then call OnDamaged on each
		gameplayobjectmanager->DoTakeDamage(this, damage, pAttacker);
		return original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);
	}
};

//-----------------
// ATTACH DETOURS
void HBEp1::AttachDetours() {
	EP1_ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));
	ScenarioPlayModeUpdateGoals_detour::attach(GetAddress(Simulator::cScenarioPlayMode, UpdateGoals));
	EP1_WalkTo_detour::attach(GetAddress(Simulator::cCreatureBase, WalkTo));
	EP1_OnJumpLand_detour::attach(GetAddress(Simulator::cCreatureAnimal, OnJumpLand));
	EP1_CombatTakeDamage_detour::attach(GetAddress(Simulator::cCombatant, TakeDamage));
}
