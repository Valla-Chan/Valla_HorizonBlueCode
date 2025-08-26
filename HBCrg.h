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

//----------
// Detours

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

// Spui loading/spawning detour
member_detour(CRG_ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey & name, bool arg_4, uint32_t arg_8) {
		if (IsCreatureGame()) {
			// rolling over a player nest
			if (name.instanceID == id("Rollover_FixedObject") && GameViewManager.mHoveredObject) {
				auto nest = object_cast<cNest>(GameViewManager.mHoveredObject);
				if (nest && nest->IsPlayerOwned()) {
					ResourceKey newSpui = ResourceKey(id("Rollover_Nest"), name.typeID, name.groupID);
					return original_function(this, newSpui, arg_4, arg_8);
				}
			}
			// loading the creature part collection tabs
			else if (name.instanceID == id("CRGPartUnlockItem")) {
				crg_partshards->TryCollectionsTabOpened();
			}
		}

		return original_function(this, name, arg_4, arg_8);
	}
};

// Detour the playanimation ID-picking func
member_detour(CRG_AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

		// Death anims: gen_death_v1 thru 4. 1 and 4 are front deaths, 2 and 3 are side deaths.
		// NOTE: this detour applies to all stages.
		if (animID == 0xF14FAA46 || animID == 0x05E37EC0 || animID == 0x05E37EC7 || animID == 0x05E37ECD) {
			auto creature = Common::GetAnimCreatureOwner(this);
			// should the creature start off dead? if so, only play the last frame of a side death anim.
			// also do this for dead epics
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
						return original_function(this, 0xCA939441, pChoice); // gen_death_blast
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
					//return original_function(this, animkey.instanceID, pChoice);
				}

			}
			// Check eating anims to store food
			else if (avatar == Common::GetAnimCreatureOwner(this) && crg_inventory->ShouldStoreFood()) {
				auto foodanim = crg_inventory->GetFoodStoreAnim(animID);
				if (foodanim != animID) {
					return original_function(this, foodanim, pChoice);
				}
			}
		}

		return original_function(this, animID, pChoice);

	}
};

// Detour the model setting func
virtual_detour(CRG_SetModel_detour, Simulator::cSpatialObject, Simulator::cSpatialObject, void(const ResourceKey&)) {
	void detoured(const ResourceKey & modelKey) {

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
		cNestPtr nest = object_cast<Simulator::cNest>(this);
		if (nest && !nest->mpHerd->mOwnedByAvatar) { // do not run on avatar species
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

// Detour the cursor setting func
member_detour(CRG_SetCursor_detour, UTFWin::cCursorManager, bool(uint32_t)) {
	bool detoured(uint32_t id) {

		// Replace cursor for interactable objects
		// TODO: move this to the interactable object manager
		// TODO: the model cursor sometimes gets stuck on
		if (IsCreatureGame()) {
			cInteractiveOrnament* object = ObjectManager.GetHoveredObject();
			if (object) {
				uint32_t cursorid = ObjectManager.GetModelCursorID(object->GetModelKey(), id);
				return original_function(this, cursorid);
			}
		}

		return original_function(this, id);
	}
};

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

// Detour an unknown unlockable func 1: sub_597BC0
member_detour(CRGunlockUnk1_detour, Simulator::cCollectableItems, void(UnkHashMap&, int, const ResourceKey&))
{
	void detoured(UnkHashMap & dst, int unk0, const ResourceKey & speciesKey)
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

virtual_detour(CRG_EditorOnEnter_detour, cEditor, cEditor, bool()) {
	bool detoured() {
		auto value = original_function(this);
		//if (this->mEditorName == id(0x465C50BA))
		App::ScheduleTask(crg_partshards, &CRG_PartShards::SuppressDestroyedParts, 0.01f);
		return value;
	}
};

// For part shards.
// TODO: move this to the part shards manager
static_detour(CRG_ImageSetBackgroundByKey_detour, bool(IWindow*, const ResourceKey&, int imageIndex)) {
	bool detoured(IWindow * pWindow, const ResourceKey & imageName, int imageIndex) {
		auto value = original_function(pWindow, imageName, imageIndex);
		// creatureparticons~
		if (imageName.groupID == 0x02231C8B) {
			auto windowIDstorange = pWindow->FindWindowByID(0x060B1041);
			if (windowIDstorange) {
				windowIDstorange->SetCommandID(imageName.instanceID);
				crg_partshards->AddPartWindow(pWindow, imageName);
			}
		}
		return value;
	}
};

// Called when a combatant takes damage
virtual_detour(CRG_CombatTakeDamage_detour, Simulator::cCombatant, Simulator::cCombatant, int(float, uint32_t, int, const Vector3&, cCombatant*))
{
	int detoured(float damage, uint32_t attackerPoliticalID, int integer, const Vector3 & vector, cCombatant * pAttacker)
	{
		// Spread disease through damage
		// TODO: make this not spread thru charge.

		// target of damage is a creature
		auto thiscreature = object_cast<cCreatureBase>(this);
		if (thiscreature) {
			// attacker is also a creature
			auto attackercreature = object_cast<cCreatureBase>(pAttacker);
			if (attackercreature && attackercreature->mbIsDiseased) {
				auto thiscreature = object_cast<cCreatureBase>(this);
				if (thiscreature && !thiscreature->mbIsDiseased) {
					thiscreature->mbIsDiseased = true;
				}
			}

			//thiscreature->PlayAnimationTo(0x128FF64A, pAttacker->ToSpatialObject());
		}

		return original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);
	}
};

//-----------------
// ATTACH DETOURS
void HBCrg::AttachDetours() {
	cCriticalHit::AttachDetours();

	CRG_ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));
	CRG_AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	CRG_SetModel_detour::attach(GetAddress(Simulator::cSpatialObject, SetModelKey));
	CRG_SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));

	CRG_AvatarScaling_detour::attach(GetAddress(Simulator::cCreatureGameData, CalculateAvatarNormalizingScale));

	CRGunlock_detour::attach(GetAddress(Simulator::CreatureGamePartUnlocking, sub_D3B460));
	//CRGunlockUnk1_detour::attach(GetAddress(Simulator::cCollectableItems, sub_597BC0));
	//CRGunlockUnk2_detour::attach(GetAddress(Simulator::cCollectableItems, sub_597390));

	CRG_EditorOnEnter_detour::attach(GetAddress(cEditor, OnEnter));
	CRG_ImageSetBackgroundByKey_detour::attach(GetAddress(UTFWin::Image, SetBackgroundByKey));

	CRG_CombatTakeDamage_detour::attach(GetAddress(Simulator::cCombatant, TakeDamage));
}
