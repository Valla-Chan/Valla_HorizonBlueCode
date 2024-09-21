// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore/Effects.h>
#include <Spore/Swarm/cEffectsManager.h>
#include <Spore\Simulator\cCreatureGameData.h>

// Cheats
#include "Cheats/BuildCRG.h"
#include "Cheats/Kill.h"
#include "Cheats/CRG_Starve.h"
#include "Cheats/CRG_BrainBoost.h"
#include "Cheats/CRG_BrainDrain.h"
#include "Cheats/CRG_SwapCreature.h"
#include "Cheats/CRG_PosessTarget.h"
#include "Cheats/CRG_ResurrectTarget.h"
#include "Cheats/CRG_Recharge.h"
#include "Cheats/CRG_SpawnPlant.h"
#include "Cheats/CRG_GrowUp.h"
#include "Cheats/PrintCursor.h"
#include "Cheats/SetCursorCheat.h"
#include "Cheats/TRG_GetTribeInfo.h"
#include "Cheats/SetChieftainColor.h"
#include "Cheats/SetGlideSpeed.h"
#include "Cheats/CRG_GetPart.h"
#include "Cheats/CRG_GetSick.h"
#include "Cheats/CRG_GiveAllParts.h"
#include "Cheats/SetTimeScale.h"
#include "Cheats/SetTime.h"
#include "Cheats/TRG_GetClosestHerd.h"
#include "Cheats/TRG_SelectMembers.h"
#include "Cheats/TRG_AddToTribe.h"

// UI
#include "UI_Timescale.h"

// CRG Ingame Behaviors
#include "CRG_EnergyHungerSync.h"
#include "CRG_WaterBehavior.h"
#include "CRG_DiseaseManager.h"
#include "CRG_AttackBasic.h"
#include "CRG_ObjectManager.h"

// TRG Ingame Behaviors
#include "TRG_CreaturePickup.h"
#include "TRG_ChieftainManager.h"
#include "TRG_MemberManager.h"
#include "TRG_IslandEventManager.h"

// EP1 Ingame Behaviors
#include "EP1_PosseCommand.h"
#include "EP1_CaptainAbilities.h"
#include "EP1_GameplayObjectManager.h"
#include "EP1_GameplayObject_HoloProjector.h"
#include "EP1_GameplayObject_DriveMarker.h"
#include "EP1_GameplayObject_IceCube.h"

// Singletons
#include "CapabilityChecker.h"

// Scripts
#include "CRE_ViewerAnims.h"
#include "CR_JetHover.h"

cObjectManager* obconverter;
TRG_ChieftainManager* chiefmanager;
CRG_DiseaseManager* diseasemanager;
TRG_CreaturePickup* trg_creaturepickup;
EP1_PosseCommand* ep1_possecommand;
EP1_CaptainAbilities* ep1_captainabilities;

TRG_IslandEventManager* trg_ieventmanager;

// manager
EP1_GameplayObjectManager* gameplayobjectmanager;
// submanagers
EP1_GameplayObject_HoloProjector* ep1_gameplayobject_projector;
EP1_GameplayObject_DriveMarker* ep1_gameplayobject_drivemarker;
EP1_GameplayObject_IceCube* ep1_gameplayobject_icecube;

void Initialize()
{
	// This method is executed when the game starts, before the user interface is shown
	CheatManager.AddCheat("Build", new(BuildCRG));
	CheatManager.AddCheat("Kill", new(Kill));
	CheatManager.AddCheat("Starve", new(CRG_Starve));
	CheatManager.AddCheat("BrainBoost", new(CRG_BrainBoost));
	CheatManager.AddCheat("BrainDrain", new(CRG_BrainDrain));
	//CheatManager.AddCheat("SwapCreature", new(CRG_SwapCreature));
	CheatManager.AddCheat("PosessTarget", new(CRG_PosessTarget));
	CheatManager.AddCheat("ResurrectTarget", new(CRG_ResurrectTarget));
	CheatManager.AddCheat("Recharge", new(CRG_Recharge));
	CheatManager.AddCheat("SpawnPlant", new(CRG_SpawnPlant));
	CheatManager.AddCheat("GrowUp", new(CRG_GrowUp));
	CheatManager.AddCheat("PrintCursor", new(PrintCursor));
	//CheatManager.AddCheat("SetCursor", new(SetCursorCheat));
	CheatManager.AddCheat("TribeInfo", new(TRG_GetTribeInfo));
	CheatManager.AddCheat("SetChieftainColor", new(SetChieftainColor));
	//CheatManager.AddCheat("SetGlideSpeed", new(SetGlideSpeed));
	CheatManager.AddCheat("GetSick", new(CRG_GetSick));
	CheatManager.AddCheat("GivePart", new(CRG_GetPart));
	CheatManager.AddCheat("GiveAllParts", new(CRG_GiveAllParts));
	CheatManager.AddCheat("TimeScale", new(SetTimeScale));
	CheatManager.AddCheat("SetTime", new(SetTime));
	CheatManager.AddCheat("GetHerd", new(TRG_GetClosestHerd));
	CheatManager.AddCheat("SelectUnits", new(TRG_SelectMembers));
	CheatManager.AddCheat("AddToTribe", new(TRG_AddToTribe));

	// UI
	UI_Timescale* uitimescale = new(UI_Timescale);

	// CRG
	CRG_EnergyHungerSync* energyhungersync = new(CRG_EnergyHungerSync);
	CRG_WaterBehavior* waterbehavior = new(CRG_WaterBehavior);
	CRG_AttackBasic* crg_attackbasic = new(CRG_AttackBasic);
	

	// TRG
	trg_creaturepickup = new(TRG_CreaturePickup);
	chiefmanager = new(TRG_ChieftainManager);
	// strategies
	SimulatorSystem.AddStrategy(new TRG_MemberManager(), TRG_MemberManager::NOUN_ID);
	trg_ieventmanager = new(TRG_IslandEventManager);
	SimulatorSystem.AddStrategy(trg_ieventmanager, TRG_IslandEventManager::NOUN_ID);


	// EP1
	ep1_possecommand = new(EP1_PosseCommand);
	WindowManager.GetMainWindow()->AddWinProc(ep1_possecommand);

	ep1_captainabilities = new(EP1_CaptainAbilities);
	WindowManager.GetMainWindow()->AddWinProc(ep1_captainabilities);

	// Initialize Manager
	gameplayobjectmanager = new(EP1_GameplayObjectManager);
	// Initialize Submanagers
	ep1_gameplayobject_projector = new(EP1_GameplayObject_HoloProjector);
	ep1_gameplayobject_icecube = new(EP1_GameplayObject_IceCube);
	// Add to manager
	gameplayobjectmanager->AddGameplayObjectSubmanager(ep1_gameplayobject_projector);
	gameplayobjectmanager->AddGameplayObjectSubmanager(ep1_gameplayobject_icecube);

	ep1_gameplayobject_drivemarker = new(EP1_GameplayObject_DriveMarker);
	WindowManager.GetMainWindow()->AddWinProc(ep1_gameplayobject_drivemarker);


	// Managers
	obconverter = new(cObjectManager);

	diseasemanager = new(CRG_DiseaseManager);


	// Singletons
	cCapabilityChecker* capchecker = new(cCapabilityChecker);

}

// This method is called when the game is closing
void Dispose()
{
	obconverter = nullptr;
	chiefmanager = nullptr;
	diseasemanager = nullptr;
	trg_creaturepickup = nullptr;

	ep1_possecommand = nullptr;
	ep1_captainabilities = nullptr;

	gameplayobjectmanager = nullptr;
	ep1_gameplayobject_projector = nullptr;
	ep1_gameplayobject_drivemarker = nullptr;
	ep1_gameplayobject_icecube = nullptr;
}

cCreatureAnimal* GetAnimCreatureOwner(const AnimatedCreaturePtr& animcreature) {
	cCombatantPtr target = nullptr;
	for (auto creature : Simulator::GetData<Simulator::cCreatureAnimal>()) { //loop through all creatures
		if (creature->mpAnimatedCreature.get() == animcreature) //if the current creature is the owner of the AnimatedCreature that triggered the event
		{
			return creature.get(); //set the crt pointer to the current creature
		}
	}
	return nullptr;
}

// Detour the animation playing func
member_detour(AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

		if (IsCreatureGame() || IsScenarioMode()) {

			// replace the jump jet animations when in mid-flight
			uint32_t jetanim = JetHover_GetAnim(animID);
			if (jetanim != animID && ep1_captainabilities->can_jumpburst) {
				return original_function(this, jetanim, pChoice);
			}


			// Scenario Social behaviors
			// detect when creature waves after being allied
			if (animID == 0xb225328f) { // "soc_hurrayclap"
				auto creature = GetAnimCreatureOwner(this);
				// if it is a sentient creature, replace the clapping with a salute
				if (creature && creature->GetCurrentBrainLevel() >= 5 && creature->mAge >= 1) {
					return original_function(this, 0x00BF10B3, pChoice); //"soc_posse_salute_01a"
				}
				
			}

			// make holograms not play the hover animation
			if (animID == 0x0692E6A9) { // "gen_jump_loop"
				auto creature = GetAnimCreatureOwner(this);
				if (creature && ep1_gameplayobject_projector->IsCreatureHologram(creature)) {
					return original_function(this, 0x04330667, pChoice); //"csa_idle_gen_editor"
				}
				
			}

			// Swap the wave for a flex intimidate.
			if (animID == 0x5403B863) { // "gen_posse_call"
				// Make this only play if avatar in combat stance mode when interacting
				// For some reason, these are opposite.
				if ((IsScenarioMode() && CreatureGameData.GetAbilityMode() != cCreatureGameData::AbilityMode::Attack) ||
					(IsCreatureGame() && CreatureGameData.GetAbilityMode() == cCreatureGameData::AbilityMode::Attack) ){
					return original_function(this, 0x0418B841, pChoice); //"csa_phpto_flex"
				}
			}
		}

		// CRG
		if (IsCreatureGame()) {
			cCreatureAnimal* avatar = GameNounManager.GetAvatar();
			if (avatar && animID == 0x03DF6DFF) { //gen_dig_hands

				cInteractiveOrnament* object = ObjectManager.FindInteractedObject();
				obconverter->SetInteractedObject(object);

				ResourceKey animkey = obconverter->GetModelInteractAnim(avatar, object->GetModelKey(), animID);

				if (animkey.instanceID != 0x0) {
					obconverter->StartWaitingForNoun();
					return original_function(this, animkey.instanceID, pChoice);
				}
				
			}
		}
		// Sporepedia viewer
		else if (GetGameModeID() == kGGEMode) {
			if (ShouldReplaceAnim(animID)) {
				animID = GetRandViewerAnim();
			}
		}

		//SporeDebugPrint("Animation: %x", animID);
		return original_function(this, animID, pChoice);

	}
};

// Detour the cursor setting func
member_detour(SetCursor_detour, UTFWin::cCursorManager, bool(uint32_t)) {
	bool detoured(uint32_t id) {


		if (IsCreatureGame()) {
			cInteractiveOrnament* object = ObjectManager.GetHoveredObject();
			if (object) {
				uint32_t cursorid = ObjectManager.GetModelCursorID(object->GetModelKey(), id);
				return original_function(this, cursorid);
			}
		}
		
		else if (IsTribeGame()) {
			// held tribe member
			if (trg_creaturepickup->held_member) {
				return original_function(this, 0x03C32077);
			}
			else {
				auto tribe = trg_creaturepickup->GetPlayerTribe();
				if (tribe) {
					auto members = tribe->GetSelectableMembers();
					for (auto member : members) {
						if (member && member->IsRolledOver()) {
							return original_function(this, BasicCursorIDs::Cursors::GrabOpen);
						}
					}
				}
			}
			// island event item hovered
			if (trg_ieventmanager->IsEventItemHovered()) {
				return original_function(this, 0x24C6D844);
			}


		}
		

		//App::ConsolePrintF("Setting cursor: %x", id);
		return original_function(this, id);

	}
};


// Detour the cursor LOADING func
member_detour(LoadCursor_detour, UTFWin::cCursorManager, bool(uint32_t, const char16_t*, bool, int, int)) {
	bool detoured(uint32_t id, const char16_t* fileName, bool loadFromFile = true, int xHotspot = 0, int yHotspot = 0) {
		return original_function(this, id, fileName, loadFromFile, xHotspot, yHotspot);
	}
};

// Detour the model setting func
virtual_detour(SetModel_detour, Simulator::cSpatialObject, Simulator::cSpatialObject, void (const ResourceKey&)) {
	void detoured(const ResourceKey& modelKey) {

		if (modelKey.instanceID == id("CRG_Meteorite")) {
			App::ConsolePrintF("CRG_Meteorite");
		}
		if (modelKey.instanceID == id("CR_Meteorite_Small")) {
			App::ConsolePrintF("CR_Meteorite_Small");
		}
		if (modelKey.instanceID == id("cr_feature_meteorite_a")) {
			App::ConsolePrintF("cr_feature_meteorite_a");
		}

		// if this is a nest, check its herd type
		cNestPtr nest = object_cast<Simulator::cNest>(this);
		if (nest && !nest->mpHerd->mOwnedByAvatar) {
			cHerdPtr herd = nest->mpHerd;

			// set nest model from override
			ResourceKey nestmodelkey = ObjectManager.GetHerdNestModel(herd->mDefinitionID);
			if (nestmodelkey.instanceID != 0x0) {
				original_function(this, nestmodelkey);
				return;
			}
		}
		original_function(this, modelKey);
	}
};

// Detour the tribe spawning func
member_detour(TribeSpawn_detour, Simulator::cTribe, void(const Vector3&, int, int, bool)) {
	void detoured(const Math::Vector3& position, int numMembers, int value, bool boolvalue) {
		App::ConsolePrintF("Tribe Spawned");
		original_function(this, position, numMembers, value, boolvalue);
		return;
	}
};

// Detour the herd spawning func
member_detour(HerdSpawn_detour, Simulator::cHerd, cHerd*(const Vector3&, cSpeciesProfile*, int, bool, int, bool)) {
	cHerd* detoured(const Vector3 & position, cSpeciesProfile* pSpeciesProfile, int herdSize, bool isOwnedByAvatar, int creaturePersonality, bool createNest)
	{
		//App::ConsolePrintF("Herd Spawned");
		cHerd* herd = original_function(this, position, pSpeciesProfile, herdSize, isOwnedByAvatar, creaturePersonality, createNest);
		return herd;
	}
};


// Detour the effect playing func
member_detour(EffectOverride_detour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId) //Detouring the function for obtaining effect indexes...
	{
		// detect if a cinematic is beginning
		if (instanceId == id("fade_to_black_1") || instanceId == id("fade_to_black_3") || instanceId == id("fade_to_black_quick")) {
			MessageManager.MessageSend(id("CinematicBegin"), nullptr);
		}

		if (IsTribeGame()) {
			// 0 = reg  1 = fish  2 = seaweed
			int swapnum = -1;

			if (instanceId == id("trg_chieftain_staff")) { swapnum = 0; }
			if (instanceId == id("trg_chieftain_staff_fish")) { swapnum = 1; }
			if (instanceId == id("trg_chieftain_staff_seaweed")) { swapnum = 2; }

			int dietvalue = 0;
			if (swapnum > -1) {
				dietvalue = chiefmanager->NextQueueItem();
				uint32_t staff_id = chiefmanager->GetStaffID(dietvalue, swapnum);

				if (staff_id != 0x0) {
					return original_function(this, staff_id, groupId);
				}
			}
			
		}
		// Print FX
		//if (instanceId != 0x0) { SporeDebugPrint("%x", instanceId); }

		return original_function(this, instanceId, groupId); //And call the original function with the new instance ID.
	}
};

// Detour a player part unlocking function
member_detour(CRGunlock_detour, Simulator::CreatureGamePartUnlocking, struct cCollectableItemID(UnkHashMap&, bool, int, int))
{
	struct cCollectableItemID detoured(UnkHashMap& unk1, bool unk2, int unk3, int unk4)
	{
		SporeDebugPrint("unlocking part! hashmap size: %x, bool: %b, ints: %i %i", unk1.size(), unk2, unk3, unk4);
		cCollectableItemID partid = original_function(this, unk1, unk2, unk3, unk4);
		SporeDebugPrint("part ID: 0x%x ! 0x%x ", partid.groupID, partid.instanceID );
		
		//if (partid.instanceID != 0xffffffff) {
		//partid.groupID = 0x887C91BB;
		//}
		
		//Simulator::GetPlayer()->mpCRGItems->AddUnlockableItem(id("ce_details_wing_04-symmetric"), 0x40626000, 4, id("ce_category_wings"), 0, 3, 0, 1.0f, id("CRG_partUnlock_detail"));
		//Simulator::GetPlayer()->mpCRGItems->AddUnlockableItemFromProp(ResourceKey(id("ce_details_wing_04-symmetric"), TypeIDs::Names::prop, 0x40626000), id("ce_category_wings"), 0, 3, 0);

		return partid;

	}
};

// Detour an unknown unlockable func 1: sub_597BC0
member_detour(CRGunlockUnk1_detour, Simulator::cCollectableItems, void(UnkHashMap&, int, const ResourceKey&))
{
	void detoured(UnkHashMap& dst, int unk0, const ResourceKey& speciesKey)
	{
		SporeDebugPrint("unknown unlock func 1 ------");
		SporeDebugPrint("int: %i, hashmap size: %i", unk0, dst.size());
		//if (speciesKey != 0) {
			//SporeDebugPrint("species: 0x%x ! 0x%x", speciesKey.groupID, speciesKey.instanceID);
		//}
		original_function(this, dst, unk0, speciesKey);
		SporeDebugPrint("int: %i, hashmap size: %i", unk0, dst.size());
		return;
	}
};

// Detour an unknown unlockable func 2: sub_597390
member_detour(CRGunlockUnk2_detour, Simulator::cCollectableItems, void(eastl::vector<int>&, struct cCollectableItemID, int))
{
	void detoured(eastl::vector<int>&dst, struct cCollectableItemID itemID, int unk0)
	{
		SporeDebugPrint("unknown unlock func 2 ------");
		for (auto i : dst) {
			SporeDebugPrint("vector value: %i", i);
		}
		SporeDebugPrint("instance: %x, group: %x,  int: %i", itemID.instanceID, itemID.groupID, unk0);
		original_function(this, dst, itemID, unk0);
		return;
	}
};

// Detour CalculateAvatarNormalizingScale
static_detour(AvatarScaling_detour, void())
{
	void detoured()
	{
		original_function();
		CreatureGameData.mAvatarNormalizingScale = (CreatureGameData.mAvatarNormalizingScale + 0.3f) / 1.3f;
	}
};

// Detour Creature WalkTo
virtual_detour(WalkTo_detour, Simulator::cCreatureAnimal, Simulator::cCreatureBase, void(int, const Vector3&, const Vector3&, float, float)) //cCreatureAnimal
{
	void detoured(int speedState, const Vector3& dstPos, const Vector3& arg_8, float goalStopDistance = 1.0f, float acceptableStopDistance = 2.0f)
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
virtual_detour(OnJumpLand_detour, Simulator::cCreatureAnimal, Simulator::cCreatureBase, void())
{
	void detoured()
	{
		ep1_captainabilities->ResetJumpCount();
		original_function(this);
	}
};

member_detour(ScenarioPlayModeUpdateGoals_detour, Simulator::cScenarioPlayMode, bool())
{
	bool detoured()
	{
		bool result = original_function(this);
		//if (this->mCurrentActIndex > 0) {
			gameplayobjectmanager->PropagateAction(EP1_GameplayObjectManager::UpdateScenarioGoals);
		//}
		return result;
	}
};

// Called when a combatant takes damage
virtual_detour(CombatTakeDamage_detour, Simulator::cCombatant, Simulator::cCombatant, int(float, uint32_t, int, const Vector3&, cCombatant*))
{
	int detoured(float damage, uint32_t attackerPoliticalID, int integer, const Vector3& vector, cCombatant* pAttacker)
	{
		// TODO: send the message of this to the gameplayobjectmanager, including the attacker and object
		// and have that class propogate it to the rest of the submanagers to check for if the object can be handled, then call OnDamaged
		gameplayobjectmanager->DoTakeDamage(this, damage, pAttacker);
		return original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);
	}
};

// Called when a combatant takes damage
member_detour(Pause_detour, Simulator::cGameTimeManager, int(TimeManagerPause))
{
	int detoured(TimeManagerPause pauseType)
	{
		return original_function(this, pauseType);
	}
};

// Spui spawning detour
member_detour(ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey& name, bool arg_4, uint32_t arg_8) {
		if (name.instanceID == id("Rollover_TribeHut") && trg_ieventmanager->IsEventItemHovered())
		{
			ResourceKey newSpui = ResourceKey(name.instanceID, name.typeID, name.groupID);
			newSpui.instanceID = id("Rollover_TribeRare");
			return original_function(this, newSpui, arg_4, arg_8);
		}
		return original_function(this, name, arg_4, arg_8);
	}
};

void AttachDetours()
{
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));
	//LoadCursor_detour::attach(GetAddress(UTFWin::cCursorManager, Load));
	EffectOverride_detour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));
	SetModel_detour::attach(GetAddress(Simulator::cSpatialObject, SetModelKey));  
	//TribeSpawn_detour::attach(Address(ModAPI::ChooseAddress(0xC92860, 0xC932F0)));
	HerdSpawn_detour::attach(GetAddress(Simulator::cGameNounManager, CreateHerd));

	CRGunlock_detour::attach(GetAddress(Simulator::CreatureGamePartUnlocking, sub_D3B460));
	AvatarScaling_detour::attach(GetAddress(Simulator::cCreatureGameData, CalculateAvatarNormalizingScale));

	WalkTo_detour::attach(GetAddress(Simulator::cCreatureBase, WalkTo));
	OnJumpLand_detour::attach(GetAddress(Simulator::cCreatureAnimal, OnJumpLand));

	ScenarioPlayModeUpdateGoals_detour::attach(GetAddress(Simulator::cScenarioPlayMode, UpdateGoals));

	// TODO: needs disk spore address!
	CombatTakeDamage_detour::attach(Address(0x00bfcf10));

	ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));

	//Pause_detour::attach(GetAddress(Simulator::cGameTimeManager, Pause));

	//CRGunlockUnk1_detour::attach(GetAddress(Simulator::cCollectableItems, sub_597BC0));
	//CRGunlockUnk2_detour::attach(GetAddress(Simulator::cCollectableItems, sub_597390));
}

// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

