#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"
// CRG Ingame Behaviors
#include "CRG_EnergyHungerSync.h"
#include "CRG_WaterBehavior.h"
#include "CRG_DiseaseManager.h"
#include "CRG_AttackBasic.h"
#include "CRG_ObjectManager.h"
#include "CRG_Inventory.h"
#include "CRG_NestManager.h"
#include "CRG_GameplayAbilities.h"
#include "CRG_MovementHelper.h"
#include "CRG_PartShards.h"

#include "cCriticalHit.h"


#define HBCrgPtr intrusive_ptr<HBCrg>

// CRG

cObjectManager* obconverter;

CRG_DiseaseManager* diseasemanager;
CRG_Inventory* crg_inventory;
CRG_PartShards* crg_partshards;


class HBCrg
{

public:

	static void Initialize() {
		// CRG
		CRG_EnergyHungerSync* energyhungersync = new(CRG_EnergyHungerSync);
		//CRG_WaterBehavior* waterbehavior = new(CRG_WaterBehavior);
		CRG_GameplayAbilities* crg_gameplayabilities = new(CRG_GameplayAbilities);
		CRG_NestManager* crg_nestmanager = new(CRG_NestManager);
		CRG_MovementHelper* crg_movementhelper = new(CRG_MovementHelper);

		CRG_AttackBasic* crg_attackbasic = new(CRG_AttackBasic);
		crg_inventory = new(CRG_Inventory);

		crg_partshards = new(CRG_PartShards);

		// Managers
		obconverter = new(cObjectManager);
		diseasemanager = new(CRG_DiseaseManager);
	}

	static void Dispose() {
		obconverter = nullptr;
		diseasemanager = nullptr;
		crg_inventory = nullptr;
	}

	static void AttachDetours();
};

//----------------
// Detour Helpers

// Since multiple detours on one func don't work, this is the new way to do it:
// USAGE: This function will be embedded into a larger detour.
// return true = end the chain and call original_function
// return false = continue to the next subfunction
static bool CRG_ReadSPUI_detour(UTFWin::UILayout* obj, ResourceKey& name, bool& arg_4, uint32_t& arg_8) {
	if (IsCreatureGame()) {
		// rolling over a player nest
		if (name.instanceID == id("Rollover_FixedObject") && GameViewManager.mHoveredObject) {
			auto nest = object_cast<cNest>(GameViewManager.mHoveredObject);
			if (nest && nest->IsPlayerOwned()) {
				name = ResourceKey(id("Rollover_Nest"), name.typeID, name.groupID);
				return true;
			}
		}
		// loading the creature part collection tabs
		else if (name.instanceID == id("CRGPartUnlockItem")) {
			crg_partshards->TryCollectionsTabOpened();
		}
	}
	return false;
}

static bool CRG_AnimOverride_detour(Anim::AnimatedCreature* obj, uint32_t& animID, int* pChoice) {
	// Death anims: gen_death_v1 thru 4. 1 and 4 are front deaths, 2 and 3 are side deaths.
		// NOTE: this detour applies to all stages.
	if (animID == 0xF14FAA46 || animID == 0x05E37EC0 || animID == 0x05E37EC7 || animID == 0x05E37ECD) {
		auto creature = Common::GetAnimCreatureOwner(obj);
		// should a creature start off dead? if so, only play the last frame of a side death anim, not the full falling over.
		// (also do this for dead epics)
		if (creature->mHerd && creature->mHerd->mArchetype == id("Carcass")) {
			// TODO
		}

		// make dying "charge" targets play this anim
		// plays a version of the "blast anim from creepy and cute where they stay down
		if (creature->mpLastAttacker) {
			auto attackerCreature = object_cast<cCreatureBase>(creature->mpLastAttacker);
			if (attackerCreature) {
				// charge ability index
				auto abilityidx = attackerCreature->GetAbilityIndexByType(31);
				// last attack was charge
				if (attackerCreature->mDefaultAttackAbilityIndex == abilityidx) {

					creature->Teleport(creature->GetPosition(), creature->GetOrientationYawTowards(attackerCreature->GetPosition()));
					animID = 0xCA939441;  // gen_death_blast
					return true;
				}
			}
		}
	}

	// CRG
	if (IsCreatureGame()) {
		cCreatureAnimal* avatar = GameNounManager.GetAvatar();

		// Interactable items are being dug up
		//gen_dig_hands
		if (avatar && animID == 0x03DF6DFF) {

			cInteractiveOrnament* object = ObjectManager.FindInteractedObject();
			obconverter->SetInteractedObject(object);

			auto newAnimID = obconverter->ChooseModelInteractSuccessFailureAnim(avatar, object->GetModelKey(), animID);
			if (newAnimID != 0x0) {
				obconverter->StartWaitingForNoun();
				animID = newAnimID;
				return true;
			}

		}
		// Check eating anims to store food
		else if (avatar == Common::GetAnimCreatureOwner(obj) && crg_inventory->ShouldStoreFood()) {
			auto foodanim = crg_inventory->GetFoodStoreAnim(animID);
			if (foodanim != animID) {
				animID = foodanim;
				return true;
			}
		}
	}
	return false;
}

// Detour the model setting func
static bool CRG_SetModel_detour(Simulator::cSpatialObject* obj, ResourceKey& modelKey) {

	if (modelKey.instanceID == id("CRG_Meteorite")) {
		App::ConsolePrintF("CRG_Meteorite");
	}
	if (modelKey.instanceID == id("CR_Meteorite_Small")) {
		App::ConsolePrintF("CR_Meteorite_Small");
	}
	if (modelKey.instanceID == id("cr_feature_meteorite_a")) {
		App::ConsolePrintF("cr_feature_meteorite_a");
	}

	// If this is a nest, check its herd type
	cNestPtr nest = object_cast<Simulator::cNest>(obj);
	if (nest && !nest->mpHerd->mOwnedByAvatar) { // do not run on avatar species
		cHerdPtr herd = nest->mpHerd;

		// set nest model from override
		ResourceKey nestmodelkey = ObjectManager.GetHerdNestModel(herd->mDefinitionID);
		if (nestmodelkey.instanceID != 0x0) {
			modelKey = nestmodelkey;
			return true;
		}
	}
	return false;
}

// Detour the cursor setting func
static bool CRG_SetCursor_detour(UTFWin::cCursorManager* obj, uint32_t& id) {
	// Replace cursor for interactable objects
	// TODO: move this code to a func in the interactable object manager?
	// TODO: the model cursor sometimes gets stuck on
	if (IsCreatureGame()) {
		cInteractiveOrnament* object = ObjectManager.GetHoveredObject();
		if (object) {
			uint32_t cursorid = ObjectManager.GetModelCursorID(object->GetModelKey(), id);
			id = cursorid;
			return true;
		}
	}

	return false;
}

// For part shards.
// TODO: move this to the part shards manager
static bool CRG_ImageSetBackgroundByKey_detour(IWindow* pWindow, const ResourceKey& imageName, int imageIndex) {
	// NOTE: run after "original_function" in the detour
	// creatureparticons~
	if (imageName.groupID == 0x02231C8B) {
		auto windowIDstorange = pWindow->FindWindowByID(0x060B1041);
		if (windowIDstorange) {
			windowIDstorange->SetCommandID(imageName.instanceID);
			crg_partshards->AddPartWindow(pWindow, imageName);
		}
	}
	return false;
}

// Called when a combatant takes damage
static bool CRG_CombatTakeDamage_detour(Simulator::cCombatant* obj, float& damage, uint32_t& attackerPoliticalID, int& integer, const Vector3& vector, cCombatant* pAttacker) {
	// Spread disease through damage. TODO: make this not spread thru charge.
	auto thiscreature = object_cast<cCreatureBase>(obj);
	// target of damage is a creature
	if (thiscreature) {
		// attacker is also a creature
		auto attackercreature = object_cast<cCreatureBase>(pAttacker);
		if (attackercreature && attackercreature->mbIsDiseased) {
			auto thiscreature = object_cast<cCreatureBase>(obj);
			if (thiscreature && !thiscreature->mbIsDiseased) {
				thiscreature->mbIsDiseased = true;
			}
		}
		//thiscreature->PlayAnimationTo(0x128FF64A, pAttacker->ToSpatialObject());
	}
	return false;
}

// Detour GetRolloverIdForObject in SimulatorRollover
static bool CRG_GetRolloverIdForObject_detour(cGameData* object, UI::SimulatorRolloverID& value) {
	if (IsCreatureGame()) {
		auto nest = object_cast<cNest>(object);
		// player owned nest
		if (nest && nest->IsPlayerOwned()) {
			value = UI::SimulatorRolloverID::FixedObject;
			return true;
		}
	}
	return false;
}

//----------
// Detours

// Detour CalculateAvatarNormalizingScale
static_detour(CRG_AvatarScaling_detour, void())
{
	void detoured()
	{
		original_function();
		CreatureGameData.mAvatarNormalizingScale = (CreatureGameData.mAvatarNormalizingScale + 0.3f) / 1.3f;
	}
};

// Detour a player part unlocking function
member_detour(CRGunlock_detour, Simulator::CreatureGamePartUnlocking, struct cCollectableItemID(UnkHashMap&, bool, int, int))
{
	struct cCollectableItemID detoured(UnkHashMap & unk1, bool firstCall, int unk3, int unlockLevel)
	{
		//SporeDebugPrint("unlocking part! hashmap size: %x, bool: %b, ints: %i %i", unk1.size(), firstCall, unk3, unlockLevel);
		cCollectableItemID unlockID = original_function(this, unk1, firstCall, unk3, unlockLevel);
		//SporeDebugPrint("part ID: 0x%x ! 0x%x ", unlockID.groupID, unlockID.instanceID );

		return unlockID;
	}
};

// Shards related
virtual_detour(CRG_EditorOnEnter_detour, cEditor, cEditor, bool()) {
	bool detoured() {
		auto value = original_function(this);
		//if (this->mEditorName == id(0x465C50BA))
		App::ScheduleTask(crg_partshards, &CRG_PartShards::SuppressDestroyedParts, 0.01f);
		return value;
	}
};

//-----------------
// ATTACH DETOURS
void HBCrg::AttachDetours() {
	cCriticalHit::AttachDetours();
	CRG_AvatarScaling_detour::attach(GetAddress(Simulator::cCreatureGameData, CalculateAvatarNormalizingScale));
	CRGunlock_detour::attach(GetAddress(Simulator::CreatureGamePartUnlocking, sub_D3B460));
	CRG_EditorOnEnter_detour::attach(GetAddress(cEditor, OnEnter));
}
