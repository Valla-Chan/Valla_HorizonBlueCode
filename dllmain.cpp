// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore/Effects.h>
#include <Spore/Swarm/cEffectsManager.h>
#include <Spore\Simulator\cCreatureGameData.h>

// Cheats
#include "Cheats/BuildCRG.h"
#include "Cheats/Kill.h"
#include "Cheats/Hurtme.h"
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
#include "Cheats/TRG_AddFood.h"
#include "Cheats/SetVehicleTool.h"
#include "Cheats/MarkInteractives.h"
#include "Cheats/SendMessage.h"
#include "Cheats/TRG_AddTribeSlot.h"
#include "Cheats/GetTribeColors.h"
#include "Cheats/Teleport.h"
#include "Cheats/Rename.h"
//#include "ListenAllMessages.h"

// UI
#include "UI_Timescale.h"

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

// TRG Ingame Behaviors
#include "TRG_CreaturePickup.h"
#include "TRG_ChieftainManager.h"
#include "TRG_MemberManager.h"
#include "TRG_IslandEventManager.h"
#include "TRG_TribePlanManager.h"
#include "TRG_TribeHutManager.h"

// CVG Ingame Behaviors
#include "CVG_CreatureManager.h"

// EP1 Ingame Behaviors
#include "EP1_PosseCommand.h"
#include "EP1_CaptainAbilities.h"
#include "EP1_GameplayObjectManager.h"
#include "EP1_GameplayObject_HoloProjector.h"
#include "EP1_GameplayObject_DriveMarker.h"
#include "EP1_GameplayObject_IceCube.h"
#include "EP1_BehaviorManager.h"

// Singletons
#include "CapabilityChecker.h"

// Scripts
#include "CRE_ViewerAnims.h"
#include "CR_JetHover.h"

cObjectManager* obconverter;

CRG_DiseaseManager* diseasemanager;
CRG_Inventory* crg_inventory;

TRG_ChieftainManager* chiefmanager;
TRG_CreaturePickup* trg_creaturepickup;
TRG_IslandEventManager* trg_ieventmanager;
TRG_TribePlanManager* trg_tribeplanmanager;
TRG_TribeHutManager* trg_hutmanager;

CVG_CreatureManager* cvg_creaturemanager;


EP1_PosseCommand* ep1_possecommand;
EP1_CaptainAbilities* ep1_captainabilities;
// manager
EP1_GameplayObjectManager* gameplayobjectmanager;
// submanagers
EP1_GameplayObject_HoloProjector* ep1_gameplayobject_projector;
EP1_GameplayObject_DriveMarker* ep1_gameplayobject_drivemarker;
EP1_GameplayObject_IceCube* ep1_gameplayobject_icecube;
EP1_BehaviorManager* ep1_behaviormanager;

void Initialize()
{
	// This method is executed when the game starts, before the user interface is shown
	CheatManager.AddCheat("Build", new(BuildCRG));
	CheatManager.AddCheat("Kill", new(Kill));
	CheatManager.AddCheat("HurtMe", new(HurtMe));
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
	CheatManager.AddCheat("TimeSet", new(SetTime));
	CheatManager.AddCheat("GetHerd", new(TRG_GetClosestHerd));
	CheatManager.AddCheat("SelectUnits", new(TRG_SelectMembers));
	CheatManager.AddCheat("AddToTribe", new(TRG_AddToTribe));
	CheatManager.AddCheat("AddFood", new(TRG_AddFood));
	CheatManager.AddCheat("SetVehicleTool", new(SetVehicleTool));
	CheatManager.AddCheat("MarkInteractives", new(MarkInteractives));
	CheatManager.AddCheat("SendMessage", new(SendMessageCheat));
	CheatManager.AddCheat("AddTribeSlot", new(TRG_AddTribeSlot));
	CheatManager.AddCheat("GetTribeColors", new(GetTribeColors));
	CheatManager.AddCheat("Teleport", new(Teleport));
	CheatManager.AddCheat("Rename", new(Rename));
	//CheatManager.AddCheat("ListenAllMessages", new(ListenAllMessages));

	// UI
	UI_Timescale* uitimescale = new(UI_Timescale);

	// CRG
	CRG_EnergyHungerSync* energyhungersync = new(CRG_EnergyHungerSync);
	//CRG_WaterBehavior* waterbehavior = new(CRG_WaterBehavior);
	CRG_GameplayAbilities* crg_gameplayabilities = new(CRG_GameplayAbilities);
	//CRG_NestManager* crg_nestmanager = new(CRG_NestManager);
	CRG_MovementHelper* crg_movementhelper = new(CRG_MovementHelper);

	CRG_AttackBasic* crg_attackbasic = new(CRG_AttackBasic);
	crg_inventory = new(CRG_Inventory);
	SimulatorSystem.AddStrategy(crg_inventory, CRG_Inventory::NOUN_ID);

	// TRG
	trg_creaturepickup = new(TRG_CreaturePickup);
	chiefmanager = new(TRG_ChieftainManager);
	trg_tribeplanmanager = new(TRG_TribePlanManager);
	// strategies
	SimulatorSystem.AddStrategy(new TRG_MemberManager(), TRG_MemberManager::NOUN_ID);
	trg_ieventmanager = new(TRG_IslandEventManager);
	SimulatorSystem.AddStrategy(trg_ieventmanager, TRG_IslandEventManager::NOUN_ID);
	trg_hutmanager = new(TRG_TribeHutManager);
	SimulatorSystem.AddStrategy(trg_hutmanager, TRG_TribeHutManager::NOUN_ID);

	// CVG
	cvg_creaturemanager = new(CVG_CreatureManager);
	SimulatorSystem.AddStrategy(cvg_creaturemanager, CVG_CreatureManager::NOUN_ID);

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

	ep1_behaviormanager = new(EP1_BehaviorManager);


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
	diseasemanager = nullptr;
	crg_inventory = nullptr;

	chiefmanager = nullptr;
	trg_creaturepickup = nullptr;
	trg_tribeplanmanager = nullptr;
	trg_hutmanager = nullptr;

	cvg_creaturemanager = nullptr;

	ep1_possecommand = nullptr;
	ep1_captainabilities = nullptr;

	gameplayobjectmanager = nullptr;
	ep1_gameplayobject_projector = nullptr;
	ep1_gameplayobject_drivemarker = nullptr;
	ep1_gameplayobject_icecube = nullptr;
	ep1_behaviormanager = nullptr;
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
			// check eating anims to store food
			else if (avatar == GetAnimCreatureOwner(this) && crg_inventory->ShouldStoreFood()) {
				auto foodanim = crg_inventory->GetFoodStoreAnim(animID);
				if (foodanim != animID) {
					return original_function(this, foodanim, pChoice);
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

			//// Only run these if not in the planner
			// ---------------------------------------
			if (!trg_creaturepickup->IsPlannerOpen()) {
				// held tribe member
				if (trg_creaturepickup->held_member) {
					return original_function(this, 0x03C32077);
				}
				else {
					// check if a member of the tribe is hovered over
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

			//// Run these any time.
			// ---------------------
			// construction plot hovered
			if (trg_tribeplanmanager->IsConstructionPlotHovered() && id == 0x525ff0f) {
				return original_function(this, 0xFA09CD25);
			}

		}
		
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

		if (IsTribeGame()) {
			auto tribetool = object_cast<cTribeTool>(this);
			if (tribetool && trg_tribeplanmanager->IsToolInProgress(tribetool)) {
				//original_function(this, trg_tribeplanmanager->plot_model01);
				auto model = trg_tribeplanmanager->GetPlotModel(tribetool);
				//SporeDebugPrint("SETTING THE TRIBE PLOT MODEL!!!!");
				original_function(this, model);
				return;
			}
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

// Detour the tribe spawning func
static_detour(TribeSpawn_detour, cTribe*(const Vector3&, int, int, int, bool, cSpeciesProfile*)) {
	cTribe* detoured(const Math::Vector3 &position, int tribeArchetype, int numMembers, int foodAmount, bool boolvalue, cSpeciesProfile* species) {
		App::ConsolePrintF("Tribe Spawned");
		if (IsCivGame()) {
			species = cvg_creaturemanager->GetRandomTribeSpecies(species);
		}
		cTribe* tribe = original_function(position, tribeArchetype, numMembers, foodAmount, boolvalue, species);
		trg_hutmanager->SetTribeName(tribe);
		
		return tribe;
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

//-----------------------------------
//// TRIBE TOOLS DETOURS
//
// Detour GetTribeToolData in cTribeToolData
static_detour(GetTribeToolData_detour, cTribeToolData*(int)) {
	cTribeToolData* detoured(int toolType) {
		// TODO: make this actually dynamic!
		// Maybe even read and store the files from the folder, like the normal code does.
		if (toolType > 11) {
			ResourceKey toolKey = ResourceKey(0x0, Names::prop, 0x04292F52);
			int typeIDoverride = -1;

			// Home hut
			// Note: The entire range of Housing objects all return the data from tool 11
			if (toolType > 11 && toolType <= 22) {
				toolKey.instanceID = id("homehut");
				typeIDoverride = toolType;
			}
			// WatchTower
			else {
				toolKey.instanceID = id("WatchTower");
			}

			return trg_tribeplanmanager->TribeToolDataFromProp(toolKey, typeIDoverride);

		}
		
		return original_function(toolType);
	}
};

// Detour GetToolClass in cTribeTool
member_detour(GetToolClass_detour, Simulator::cTribeTool, int()) {
	int detoured() {
		if (this->GetToolType() > 11) {
			return 1;
		}
		return original_function(this);
	}
};

// Detour GetRolloverIdForObject in SimulatorRollover
static_detour(GetRolloverIdForObject_detour, UI::SimulatorRolloverID(Simulator::cGameData*)) {
	UI::SimulatorRolloverID detoured(Simulator::cGameData* object) {
		//auto tribetool = object_cast<cTribeTool>(object);
		auto value = original_function(object);
		return value;
	}
};

// Detour CreateTool in cTribe
member_detour(CreateTool_detour, Simulator::cTribe, cTribeTool* (int)) {
	cTribeTool* detoured(int toolType) {
		auto tool = original_function(this, toolType);
		trg_tribeplanmanager->AddedTool(tool, this);

		return tool;
	}
};


//-----------------------------------

// Detour the UIEventLog ShowEvent func
member_detour(UIShowEvent_detour, Simulator::cUIEventLog, uint32_t(uint32_t, uint32_t, int, Math::Vector3*, bool, int))
{
	uint32_t detoured(uint32_t instanceID, uint32_t groupID, int int1, Math::Vector3* vector3, bool dontAllowDuplicates, int int2)
	{
		// If this is a warning notif in tribal stage, do not run the func unless player tribe has a watchtower.
		if (IsTribeGame() && (
			instanceID == id("WildAnimalsRaiding") ||
			instanceID == id("raiderscoming") ||
			instanceID == id("Foodraiderscoming") )) {

			// if no watchtower, return before sending the warning.
			if (!GameNounManager.GetPlayerTribe() || !GameNounManager.GetPlayerTribe()->GetToolByType(TRG_TribePlanManager::ToolTypes::Watchtower)) {
				return 0x0;
			}
		}
		// fire OG func
		auto value = original_function(this, instanceID, groupID, int1, vector3, dontAllowDuplicates, int2);

		// New city has appeared in civ
		if (IsCivGame() && instanceID == id("NewCityAppears")) {
			Simulator::ScheduleTask(cvg_creaturemanager, &CVG_CreatureManager::NewCityAppeared, 0.000000001f);
		}

		return value;
	}
};

// Detour the effect playing func
member_detour(EffectOverride_detour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId) // Detour the function for obtaining effect indexes
	{
		// detect if a cinematic is beginning and emit a message
		if (instanceId == id("fade_to_black_1") || instanceId == id("fade_to_black_3") || instanceId == id("fade_to_black_quick")) {
			MessageManager.MessageSend(id("CinematicBegin"), nullptr);
		}

		// detect if a baby is growing up and send a message
		if (instanceId == 0x3A616FEE) {
			MessageManager.MessageSend(id("BabyGrowUp"), nullptr);
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

		return original_function(this, instanceId, groupId); // Call the original function with the new instance ID.
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
		// send the message of this to the gameplayobjectmanager, including the attacker and object
		// and have that class propogate it to the rest of the submanagers to check for if the object can be handled, then call OnDamaged on each
		gameplayobjectmanager->DoTakeDamage(this, damage, pAttacker);

		// spread disease through damage
		// TODO: make this not spread thru charge.
		auto attackercreature = object_cast<cCreatureBase>(pAttacker);
		if (attackercreature && attackercreature->mbIsDiseased) {
			auto thiscreature = object_cast<cCreatureBase>(this);
			if (thiscreature && !thiscreature->mbIsDiseased) {
				thiscreature->mbIsDiseased = true;
			}
		}
		return original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);
	}
};

// Spui loading/spawning detour
member_detour(ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey& name, bool arg_4, uint32_t arg_8) {
		if (IsTribeGame()) {

			bool updateTribePopUI = false;
			// update the population UI if a tribe hut is hovered
			if (name.instanceID == id("Rollover_TribeHut") || name.instanceID == id("Rollover_Tribe_Minimap")) {
				//trg_tribeplanmanager->UpdateTribePopulationUI();
				Simulator::ScheduleTask(trg_tribeplanmanager, &TRG_TribePlanManager::UpdateTribePopulationUI, 0.000000001f);
			}

			// rolling over a tribe hut that is actually a rare item
			if (name.instanceID == id("Rollover_TribeHut") && trg_ieventmanager->IsEventItemHovered())
			{
				ResourceKey newSpui = ResourceKey(id("Rollover_TribeRare"), name.typeID, name.groupID);
				return original_function(this, newSpui, arg_4, arg_8);
			}
			// rolling over a tribe tool
			else if (name.instanceID == id("Rollover_TribeTool") && trg_tribeplanmanager->IsConstructionPlotHovered()) {
				ResourceKey newSpui = ResourceKey(id("Rollover_TribeTool_Construction"), name.typeID, name.groupID);
				return original_function(this, newSpui, arg_4, arg_8);
			}
			// rolling over a chieftain
			else if (name.instanceID == id("Rollover_TribeCitizen") && GameViewManager.mHoveredObject) {
				auto citizen = object_cast<cCreatureCitizen>(GameViewManager.mHoveredObject);
				if (citizen && citizen->mSpecializedTool == 11) {
					ResourceKey newSpui = ResourceKey(id("Rollover_TribeChieftain"), name.typeID, name.groupID);
					return original_function(this, newSpui, arg_4, arg_8);
				}
			}

		}
		else if (IsScenarioMode()) {
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

//-----------------------------------
//// EDITOR LOADING

using namespace Editors;
int trg_last_category = 0;
bool trg_has_set_category = false;

// Editor parts palette loading func, PaletteUI::Load
member_detour(PaletteUILoad_detour, Palettes::PaletteUI, void(Palettes::PaletteMain*, UTFWin::IWindow*, bool, Palettes::PaletteInfo*)) {
	void detoured(Palettes::PaletteMain * pPalette, UTFWin::IWindow * pWindow, bool bool1, Palettes::PaletteInfo * pInfo) {
		trg_has_set_category = false;
		original_function(this, pPalette, pWindow, bool1, pInfo);

		// Tribal
		if (IsTribeGame()) {
			// Add rename UI
			if (this && !GetEditor()->mpEditorNamePanel) {
				auto window = WindowManager.GetMainWindow();
				auto nameslot = window->FindWindowByID(0x272EB68E);

				// remove any existing naming UIs
				auto nameWindowOld = nameslot->FindWindowByID(0x272EB68E);
				if (nameWindowOld) {
					nameslot->DisposeWindowFamily(nameWindowOld);
				}
				// create new naming UI
				auto namepanel = GetEditor()->mpEditorNamePanel;
				namepanel = new(EditorNamePanel);
				namepanel->Initialize(GameNounManager.GetPlayerTribe(), nameslot, id("EditorNameDescribe-NoTag"), true, 0x0);

				MessageManager.MessageSend(id("TribeNameUpdated"), nullptr);
			}
			else if (!this) {
				MessageManager.MessageSend(id("TribeNameUpdated"), nullptr);
			}
		}
		else {
			trg_has_set_category = false;
		}
	}
};

// PaletteUI::SetActiveCategory
member_detour(PaletteUISetActiveCategory_detour, Palettes::PaletteUI, void(int)) {
	void detoured(int categoryIndex) {
		// load normally and store last page
		if (!IsTribeGame() || trg_has_set_category) {
			original_function(this, categoryIndex);
			if (IsTribeGame()) {
				trg_last_category = categoryIndex;
			}
			else {
				trg_last_category = 0;
			}
		}
		// load last page
		else {
			original_function(this, trg_last_category);
			Simulator::ScheduleTask(this, &PaletteUISetActiveCategory_detour::EnableHasSetCategory, 0.0001f);
		}

	}
	void PaletteUISetActiveCategory_detour::EnableHasSetCategory() {
		trg_has_set_category = true;
		MessageManager.MessageSend(id("UpdateHut"), nullptr);
		MessageManager.MessageSend(id("UpdateHomes"), nullptr);

	}
};

//-----------------------------------

// Specialized citizen role names

using Fixed32StringType = eastl::fixed_string<char16_t, 32>;
static_detour(CitizenGetSpecializedName_detour, Fixed32StringType* (Fixed32StringType*, cGameData*)) {
	Fixed32StringType* detoured(Fixed32StringType * type, cGameData * object) {

		auto citizen = object_cast<cCreatureCitizen>(object);
		// only run on non-player tribe chieftains
		if (citizen && citizen->mSpecializedTool == 11 && citizen->mpOwnerTribe != GameNounManager.GetPlayerTribe()) {

			*type = (trg_hutmanager->GetChieftainNameString(citizen).c_str());
			return type;
		}

		return original_function(type, object);;
	}
};

member_detour(CitySpawnVehicle_detour, Simulator::cCity, cVehicle* (VehiclePurpose speciality, VehicleLocomotion locomotion, struct ResourceKey key, bool isSpaceStage)) {
	cVehicle* detoured(VehiclePurpose speciality, VehicleLocomotion locomotion, struct ResourceKey key, bool isSpaceStage) {
		auto vehicle = original_function(this, speciality, locomotion, key, isSpaceStage);
		App::ScheduleTask(this, &CitySpawnVehicle_detour::SetVehicleScale, 0.0001f);

		return vehicle;
	}
	void CitySpawnVehicle_detour::SetVehicleScale() {
		auto vehicles = GetDataByCast<cVehicle>();
		if (vehicles.size() == 0) { return; }
		auto vehicle = vehicles[vehicles.size() - 1];
		if (vehicle) {
			vehicle->SetScale(vehicle->mScale * 1.7f);
		}
	}
};

//-----------------------------------


void AttachDetours()
{
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));
	EffectOverride_detour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));
	SetModel_detour::attach(GetAddress(Simulator::cSpatialObject, SetModelKey));  
	TribeSpawn_detour::attach(GetAddress(Simulator, SpawnNpcTribe));
	HerdSpawn_detour::attach(GetAddress(Simulator::cGameNounManager, CreateHerd));

	//CRGunlock_detour::attach(GetAddress(Simulator::CreatureGamePartUnlocking, sub_D3B460));
	AvatarScaling_detour::attach(GetAddress(Simulator::cCreatureGameData, CalculateAvatarNormalizingScale));

	WalkTo_detour::attach(GetAddress(Simulator::cCreatureBase, WalkTo));
	OnJumpLand_detour::attach(GetAddress(Simulator::cCreatureAnimal, OnJumpLand));

	ScenarioPlayModeUpdateGoals_detour::attach(GetAddress(Simulator::cScenarioPlayMode, UpdateGoals));

	// TODO: needs disk spore address!
	CombatTakeDamage_detour::attach(Address(0x00bfcf10));

	ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));
	PaletteUILoad_detour::attach(GetAddress(Palettes::PaletteUI, Load));
	PaletteUISetActiveCategory_detour::attach(GetAddress(Palettes::PaletteUI, SetActiveCategory));

	// Tribetools
	GetTribeToolData_detour::attach(GetAddress(Simulator, GetTribeToolData));
	GetToolClass_detour::attach(GetAddress(Simulator::cTribeTool, GetToolClass));
	GetRolloverIdForObject_detour::attach(GetAddress(UI::SimulatorRollover, GetRolloverIdForObject));
	CreateTool_detour::attach(GetAddress(Simulator::cTribe, CreateTool));

	UIShowEvent_detour::attach(GetAddress(Simulator::cUIEventLog, ShowEvent));

	// Creature Names
	CitizenGetSpecializedName_detour::attach(GetAddress(Simulator::cCreatureCitizen, GetSpecializedName));

	CitySpawnVehicle_detour::attach(GetAddress(Simulator::cCity, SpawnVehicle));

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

