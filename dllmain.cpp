// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore/Effects.h>
#include <Spore/Swarm/cEffectsManager.h>

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

// CRG Ingame Behaviors
#include "CRG_EnergyHungerSync.h"
#include "CRG_WaterBehavior.h"
#include "CRG_DiseaseManager.h"
#include "CRG_AttackBasic.h"

// Singletons
#include "CapabilityChecker.h"
#include "CRG_ObjectManager.h"
#include "TRG_ChieftainManager.h"

// Scripts
#include "CRE_ViewerAnims.h"

cObjectManager* obconverter;
TRG_ChieftainManager* chiefmanager;
CRG_DiseaseManager* diseasemanager;

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

	// CRG
	CRG_EnergyHungerSync* energyhungersync = new(CRG_EnergyHungerSync);
	CRG_WaterBehavior* waterbehavior = new(CRG_WaterBehavior);
	CRG_AttackBasic* attackbasic = new(CRG_AttackBasic);
	WindowManager.GetMainWindow()->AddWinProc(attackbasic);

	// Managers
	obconverter = new(cObjectManager);
	chiefmanager = new(TRG_ChieftainManager);
	MessageManager.AddListener(chiefmanager, kMsgCombatantKilled); //make the Ability Manager function
	MessageManager.AddListener(chiefmanager, id("TRG_GetTool")); //make the Ability Manager function

	diseasemanager = new(CRG_DiseaseManager);

	// Singletons
	cCapabilityChecker* capchecker = new(cCapabilityChecker);
}


// Detour the animation playing func
member_detour(AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

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
				App::ConsolePrintF("Replace that thang");
				animID = GetRandViewerAnim();
			}
		}

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
		//App::ConsolePrintF("Setting cursor: %x", id);
		return original_function(this, id);

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
member_detour(TribeSpawn_detour, Simulator::cTribe, void(const Math::Vector3&, int, int, bool)) {
	void detoured(const Math::Vector3& position, int numMembers, int value, bool boolvalue) {
		//App::ConsolePrintF("Tribe Spawned");
		original_function(this, position, numMembers, value, boolvalue);
		return;
	}
};

// Detour the herd spawning func
member_detour(HerdSpawn_detour, Simulator::cHerd, cHerd*(const Vector3&, cSpeciesProfile*, int, bool, int, bool)) {
	cHerd* detoured(const Vector3 & position, cSpeciesProfile* pSpeciesProfile, int herdSize, bool isOwnedByAvatar, int creaturePersonality, bool createNest)
	{
		App::ConsolePrintF("Herd Spawned");
		cHerd* herd = original_function(this, position, pSpeciesProfile, herdSize, isOwnedByAvatar, creaturePersonality, createNest);
		return herd;
	}
};


// Detour the effect playing func
member_detour(EffectOverride_detour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId) //Detouring the function for obtaining effect indexes...
	{
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

void AttachDetours()
{
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));
	EffectOverride_detour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));
	SetModel_detour::attach(GetAddress(Simulator::cSpatialObject, SetModelKey));  
	//TribeSpawn_detour::attach(Address(ModAPI::ChooseAddress(0xC92860, 0xC932F0)));
	HerdSpawn_detour::attach(GetAddress(Simulator::cGameNounManager, CreateHerd));

	CRGunlock_detour::attach(GetAddress(Simulator::CreatureGamePartUnlocking, sub_D3B460));

	//CRGunlockUnk1_detour::attach(GetAddress(Simulator::cCollectableItems, sub_597BC0));
	//CRGunlockUnk2_detour::attach(GetAddress(Simulator::cCollectableItems, sub_597390));
}

void Dispose()
{
	obconverter = nullptr;
	chiefmanager = nullptr;
	diseasemanager = nullptr;
	// This method is called when the game is closing
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

