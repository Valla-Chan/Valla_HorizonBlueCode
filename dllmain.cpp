// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore/Effects.h>
#include <Spore/Swarm/cEffectsManager.h>
#include <Spore\Simulator\cCreatureGameData.h>

// Cheats
#include "HBdebug.h"
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
#include "TRG_UseTribalTool.h"
#include "Cheats/SetVehicleTool.h"
#include "Cheats/MarkInteractives.h"
#include "Cheats/SendMessage.h"
#include "Cheats/TRG_AddTribeSlot.h"
#include "Cheats/GetTribeColors.h"
#include "Cheats/Teleport.h"
#include "Cheats/Rename.h"
#include "Cheats/SetTechLevel.h"
#include "Cheats/RepairAll.h"
#include "Cheats/SetEmpireColor.h"
#include "Cheats/HurtTarget.h"
//#include "ListenAllMessages.h"

// UI
#include "UI_Timescale.h"
#include "UI_RenamePlanet.h"

// CLG Ingame Behaviors
#include "CLG_CellController.h"

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

// TRG
#include "HBTribe.h"

// CVG Ingame Behaviors
#include "CVG_CreatureManager.h"
#include "CVG_CityWalls.h"

// SPG Tool Strats
#include "ToolFrenzyBomb.h"

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
#include "CreatureSpeedBoost.h"

CLG_CellController* cellcontroller;

cObjectManager* obconverter;

CRG_DiseaseManager* diseasemanager;
CRG_Inventory* crg_inventory;
CRG_PartShards* crg_partshards;

CVG_CreatureManager* cvg_creaturemanager;
CVG_CityWalls* cvg_citywalls;


EP1_PosseCommand* ep1_possecommand;
EP1_CaptainAbilities* ep1_captainabilities;
// manager
EP1_GameplayObjectManager* gameplayobjectmanager;
// submanagers
EP1_GameplayObject_HoloProjector* ep1_gameplayobject_projector;
EP1_GameplayObject_DriveMarker* ep1_gameplayobject_drivemarker;
EP1_GameplayObject_IceCube* ep1_gameplayobject_icecube;
EP1_BehaviorManager* ep1_behaviormanager;

cCreatureCitizenPtr last_named_citizen;
cCommEvent* pLastEvent = nullptr;

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
	CheatManager.AddCheat("Resurrect", new(CRG_ResurrectTarget));
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
	CheatManager.AddCheat("UseTribeTool", new(TRG_UseTribalTool));
	CheatManager.AddCheat("SetVehicleTool", new(SetVehicleTool));
	CheatManager.AddCheat("MarkInteractives", new(MarkInteractives));
	CheatManager.AddCheat("SendMessage", new(SendMessageCheat));
	CheatManager.AddCheat("AddTribeSlot", new(TRG_AddTribeSlot));
	CheatManager.AddCheat("GetTribeColors", new(GetTribeColors));
	CheatManager.AddCheat("Teleport", new(Teleport));
	CheatManager.AddCheat("Rename", new(Rename));
	CheatManager.AddCheat("SetTechLevel", new(SetTechLevel));
	CheatManager.AddCheat("RepairAll", new(RepairAll));
	CheatManager.AddCheat("SetEmpireColor", new(SetEmpireColor));
	CheatManager.AddCheat("HurtTarget", new(HurtTarget));
	//CheatManager.AddCheat("ListenAllMessages", new(ListenAllMessages));

#ifdef _DEBUG
	CheatManager.AddCheat("HBdebug", new(HBdebug));
# endif



	// UI / General Simulator
	UI_Timescale* uitimescale = new(UI_Timescale);
	UI_RenamePlanet* uirenameplanet = new(UI_RenamePlanet);

	// singleton: CreatureSpeedBoost
	auto creaturespeedboost = new(CreatureSpeedBoost);


	// CLG
	cellcontroller = new(CLG_CellController);

	// CRG
	CRG_EnergyHungerSync* energyhungersync = new(CRG_EnergyHungerSync);
	//CRG_WaterBehavior* waterbehavior = new(CRG_WaterBehavior);
	CRG_GameplayAbilities* crg_gameplayabilities = new(CRG_GameplayAbilities);
	CRG_NestManager* crg_nestmanager = new(CRG_NestManager);
	CRG_MovementHelper* crg_movementhelper = new(CRG_MovementHelper);

	CRG_AttackBasic* crg_attackbasic = new(CRG_AttackBasic);
	crg_inventory = new(CRG_Inventory);


	crg_partshards = new(CRG_PartShards);

	// TRG
	HBTribe::Initialize();


	// CVG
	cvg_citywalls = new(CVG_CityWalls);
	cvg_creaturemanager = new(CVG_CreatureManager);
	SimulatorSystem.AddStrategy(cvg_creaturemanager, CVG_CreatureManager::NOUN_ID);

	// SPG
	ToolManager.AddStrategy(new ToolFrenzyBomb(), id("ToolFrenzyBomb"));

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


	// Managers
	obconverter = new(cObjectManager);

	diseasemanager = new(CRG_DiseaseManager);


	// Singletons
	cCapabilityChecker* capchecker = new(cCapabilityChecker);
	
}

// This method is called when the game is closing
void Dispose()
{
	
	cellcontroller = nullptr;

	obconverter = nullptr;
	diseasemanager = nullptr;
	crg_inventory = nullptr;

	cvg_creaturemanager = nullptr;
	cvg_citywalls = nullptr;

	ep1_possecommand = nullptr;
	ep1_captainabilities = nullptr;

	gameplayobjectmanager = nullptr;
	ep1_gameplayobject_projector = nullptr;
	ep1_gameplayobject_drivemarker = nullptr;
	ep1_gameplayobject_icecube = nullptr;
	ep1_behaviormanager = nullptr;

	last_named_citizen = nullptr;
	pLastEvent = nullptr;
	
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

cCreatureCitizen* GetAnimCreatureCitizenOwner(const AnimatedCreaturePtr& animcreature) {
	cCombatantPtr target = nullptr;
	for (auto creature : Simulator::GetData<Simulator::cCreatureCitizen>()) { //loop through all creatures
		if (creature->mpAnimatedCreature.get() == animcreature) //if the current creature is the owner of the AnimatedCreature that triggered the event
		{
			return creature.get(); //set the crt pointer to the current creature
		}
	}
	return nullptr;
}

cCreatureBase* GetAnimCreatureBaseOwner(const AnimatedCreaturePtr& animcreature) {
	cCombatantPtr target = nullptr;
	for (auto creature : Simulator::GetData<Simulator::cCreatureBase>()) { //loop through all creatures
		if (creature->mpAnimatedCreature.get() == animcreature) //if the current creature is the owner of the AnimatedCreature that triggered the event
		{
			return creature.get(); //set the crt pointer to the current creature
		}
	}
	return nullptr;
}


// Detour the playanimation ID-picking func
member_detour(AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

		auto thing = this;

		// death anims: gen_death_v1 thru 4. 1 and 4 are front deaths, 2 and 3 are side deaths.
		if (animID == 0xF14FAA46 || animID == 0x05E37EC0 || animID == 0x05E37EC7 || animID == 0x05E37ECD) {
			auto creature = GetAnimCreatureOwner(this);
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

		if (IsTribeGame() || IsScenarioMode()) {
			// Jump fix
			if (animID == 0x05261D56) { // gen_hover_jump
				auto creature = GetAnimCreatureCitizenOwner(this);
				if (CapabilityChecker.GetCapabilityLevel(creature, 0x04F4E1B4) == 0) { //modelCapabilityGlide
					return original_function(this, 0x025B3BC0, pChoice);
				}
			}
		}

		if (IsTribeGame()) {

			// Getting tool from rack
			if (animID == 0x03CDACC0) { // get_tool_rack
				auto creature = GetAnimCreatureCitizenOwner(this);
				creature->mCurrentHandheldItem = 0;
			}
			// Getting/putting away tool from inventory
			else if (animID == 0x02C39200) { // get_tool
				bool suppress = false;
				auto creature = GetAnimCreatureCitizenOwner(this);
				
				if (creature) {

					// if this creature is holding a custom tool, and is en-route to attack, suppress the ability to put away this tool.
					if (creature->mSpecializedTool > HomeEnd && creature->mpCombatantTarget && creature->mpCombatantTarget->GetPoliticalID() != creature->mPoliticalID) {
						auto meta = TribeToolManager.GetTribeToolMetadata(creature->mSpecializedTool);
						if (meta && meta->mHandHeldIndex == creature->mCurrentHandheldItem) {
							suppress = true;
						}
					}

					if (!suppress && creature->IsSelected() && creature->mpOwnerTribe == GameNounManager.GetPlayerTribe()) {
						// only suppress if this is a creature that is going towards the event item
						if (TribePlanManager.trg_ieventmanager->IsCreatureActivator(creature)) {
							suppress = true;
						}
					}
					
				}
				if (suppress) {
					return original_function(this, 0x0, pChoice);
				}
				else {
					// the city protest sign is not used in tribal, so it can be used as a dummy item to ensure the value will always have to change, even if going to 0.
					creature->mCurrentHandheldItem = kHandheldItemCityProtestSignHunger;
				}	
			}
			// En Route to attack
			// TODO: fix tool disappearing when attacking buildings!
			else if (animID >= 0x0004CFFB && animID <= 0x0004CFFD) {
				auto creature = GetAnimCreatureCitizenOwner(this);
				// if this creature is holding a custom tool, and is en-route to attack, use its enroute anim instead.
				if (creature->mSpecializedTool > HomeEnd && creature->mpCombatantTarget && creature->mpCombatantTarget->GetPoliticalID() != creature->mPoliticalID) {
					auto meta = TribeToolManager.GetTribeToolMetadata(creature->mSpecializedTool);
					if (meta && meta->mToolEnRouteAnim != 0x0) { // && meta->mHandHeldIndex == creature->mCurrentHandheldItem
						
						// This causes glitches and currently fixes no issue
						//if (meta->mHandHeldIndex != creature->mCurrentHandheldItem) {
						//	creature->mCurrentHandheldItem = meta->mHandHeldIndex;
						//}
						return original_function(this, meta->mToolEnRouteAnim, pChoice);
					}
				}
			}

			// Chieftain attacks
			else if (animID == 0x05766A10 || animID == 0x05766A11) { // chief_attack_01
				if (TribeMemberManager.trg_chiefmanager->mbStaffSingleSided) {
					auto creature = GetAnimCreatureCitizenOwner(this);
					if (creature && creature == GameNounManager.GetPlayerTribe()->GetLeaderCitizen()) {

						return original_function(this, 0x056D2CF2, pChoice); // axe_attack

					}
				}
			}
			// Chieftain fishing
			else if ((animID >= 0x02C39231 && animID <= 0x02C39244) || animID == 0x05f08fa3) {
				if (TribeMemberManager.trg_chiefmanager->mbStaffSingleSided) {
					auto creature = GetAnimCreatureCitizenOwner(this);
					if (creature && creature == GameNounManager.GetPlayerTribe()->GetLeaderCitizen()) {

						switch (animID) {
							case 0x02C3923B: // chief_fish_spear
								return original_function(this, 0x83F8973B, pChoice);
							case 0x02C39244: // chief_fish_dump
								return original_function(this, 0x83F89744, pChoice);
							//
							case 0x02C39231: // chief_fish_idle_start
								return original_function(this, 0x02C39211, pChoice);
							case 0x02C39236: // chief_fish_idle
								return original_function(this, 0x02C39216, pChoice);
							case 0x02C39240: // chief_fish_succ_1
								return original_function(this, 0x02C39220, pChoice);
							case 0x02C39241: // chief_fish_succ_2
								return original_function(this, 0x02C39221, pChoice);
							case 0x02C39242: // chief_fish_succ_3
								return original_function(this, 0x02C39222, pChoice);
							case 0x02C39243: // chief_fish_fail
								return original_function(this, 0x02C39223, pChoice);
							//case 0x05f08fa3:
							//	return original_function(this, 0x83F8973B, pChoice);
						}

					}
				}
			}

			// finished eating bone treat
			else if (animID == 0xD20446B0) { // mot_carcass_eat_stop 
				auto creature = GetAnimCreatureOwner(this);

				// restore health and hunger
				creature->SetHealthPoints(creature->GetMaxHitPoints());
				creature->mHunger = 100.0f;
				// TODO: not working.
				//RelationshipManager.ApplyRelationship(creature->mHerd->mPoliticalID, GameNounManager.GetPlayerTribe()->GetPoliticalID(), kRelationshipEventTribeGift);
			}

			// Marshmallow
			else if (animID == 0x06426C10) { // vig_roastmarshmallow
				auto creature = GetAnimCreatureCitizenOwner(this);
				if (creature) {

					// randomize whether to use an alternate roast anim
					if (randf() >= 0.5) {
						// TODO: Fixme!
						int herb = CapabilityChecker.GetCapabilityLevel(creature, 0x022E785C);
						if (herb > 0 /*creature->IsHervibore()*/) {
							int carn = CapabilityChecker.GetCapabilityLevel(creature, 0x022E7847);
							
							// omnivore
							if (carn > 0) {
								if (randf() > 0.5) {
									return original_function(this, 0x896B933A, pChoice); //vig_roasthotdog
								}
								else {
									return original_function(this, 0x896B933B, pChoice); //vig_roasttoast
								}
							}
							// herbivore
							else {
								return original_function(this, 0x896B933B, pChoice); //vig_roasttoast
							}							
						}
						else {
							return original_function(this, 0x896B933A, pChoice); //vig_roasthotdog
						}
					}
				}
			}

			// Fire Dance start and end
			else if (animID == TRG_FireDanceManager::DanceAnims::soc_fire_dance_turn_fast) {
				auto creature = GetAnimCreatureCitizenOwner(this);
				if (creature) {
					TribePlanManager.trg_firedancemanager->AddDancer(creature);
				}
			}
			else if (animID == TRG_FireDanceManager::DanceAnims::soc_celebrate_trg) {
				auto creature = GetAnimCreatureCitizenOwner(this);
				if (creature) {
					TribePlanManager.trg_firedancemanager->HitLastDanceAnim(creature);
				}
			}

		}

		else if (IsCreatureGame() || IsScenarioMode()) {

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
				if ((IsScenarioMode() && CreatureGameData.GetAbilityMode() == cCreatureGameData::AbilityMode::Attack) ||
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


		// DEBUG:
//#ifdef DEBUG
		if (GameNounManager.GetPlayerTribe() ) {
			
			auto creature = GetAnimCreatureCitizenOwner(this);
			// only check anims on the chieftain!
			//if (GameNounManager.GetPlayerTribe()->GetLeaderCitizen() == creature) {

				// only check anims on the player tribe members
				if (creature && creature->mpOwnerTribe == GameNounManager.GetPlayerTribe()) {

					//SporeDebugPrint("Animation: %x", animID);
				}
			//}
			
		}
		auto hovered = GameViewManager.GetHoveredObject();
		auto hoveredAnimal = object_cast<cCreatureAnimal>(hovered);
		if (hoveredAnimal) {
			auto creature = GetAnimCreatureOwner(this);
			if (creature == hoveredAnimal) {
				//SporeDebugPrint("Hovered Creature Animation: %x", animID);
			}
			
		}

//#endif // DEBUG	
		
		//SporeDebugPrint("Animation: %x", animID);
		return original_function(this, animID, pChoice);

	}
};

// Detour the cursor setting func
member_detour(SetCursor_detour, UTFWin::cCursorManager, bool(uint32_t)) {
	bool detoured(uint32_t id) {

		// TODO: the model cursor sometimes gets stuck on
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
			if (!TribeMemberManager.trg_creaturepickup->IsPlannerOpen()) {
				// held tribe member
				if (TribeMemberManager.trg_creaturepickup->held_member) {
					return original_function(this, 0x03C32077);
				}
				else {
					// check if a member of the tribe is hovered over
					auto tribe = TribeMemberManager.trg_creaturepickup->GetPlayerTribe();
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
				if (TribePlanManager.trg_ieventmanager->IsEventItemHovered() && TribePlanManager.trg_ieventmanager->mouse_state_valid) {
					return original_function(this, 0x24C6D844);
				}
			}
			// Planner IS open
			else {
				if (TribeToolManager.HoveringScalableObject()) {
					return original_function(this, 0xE84563D4);	
				}
			}

			//// Run these any time.
			// ---------------------
			// construction plot hovered
			if (TribeToolManager.IsConstructionPlotHovered() && id == 0x525ff0f) {
				return original_function(this, 0xFA09CD25);
			}

		}
		// fix the deny spice geyser issue
		else if (IsCivGame() && id == 0x3a204b0) {
			auto hovered = GameViewManager.GetHoveredObject();
			if (hovered) {
				cCommodityNodePtr geyser = object_cast<cCommodityNode>(hovered);
				if (geyser && geyser->mPoliticalID != GameNounManager.GetPlayerCivilization()->GetPoliticalID()) {
					return original_function(this, BasicCursorIDs::Cursors::ClaimSpice);
				}
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
			if (tribetool && TribeToolManager.IsToolInProgress(tribetool)) {
				//original_function(this, cTribePlanManager->plot_model01);
				auto model = TribeToolManager.GetPlotModel(tribetool);
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
		SporeDebugPrint("Tribe Spawned");
		if (IsCivGame()) {
			species = cvg_creaturemanager->GetRandomTribeSpecies(species);
		}
		cTribe* tribe = original_function(position, tribeArchetype, numMembers, foodAmount, boolvalue, species);
		TribePlanManager.trg_hutmanager->SetupNewTribe(tribe);
		
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

		// TODO: herd variety
	}
};

// Detour the tribe member spawning func
member_detour(TribeSpawnMember_detour, Simulator::cTribe, cCreatureCitizen* (int)) {
	cCreatureCitizen* detoured(int integer) {
		this->mSpeciesKeys[0] = ResourceKey(0x06577404, TypeIDs::Names::crt, 0x40626200);
		cCreatureCitizen* ciziten = original_function(this, integer);
		if (ciziten) {
			ciziten->mbColorIsIdentity = true;
			TribeMemberManager.StoreCurrentBabies();
		}

		return ciziten;
	}
};

//-----------------------------------
//// TRIBE TOOLS DETOURS
//
// Detour GetTribeToolData in cTribeToolData
static_detour(GetTribeToolData_detour, cTribeToolData* (int)) {
	cTribeToolData* detoured(int toolType) {

		auto data = TribeToolManager.GetTribeToolData(toolType);
		if (data) {
			// Exception for tribal rares:
			// Pull data from random model file
			if (data->mToolType == ToolTypes::EventRare) {
				auto model = TribePlanManager.trg_ieventmanager->GetEventItemModelKey();
				data->mToolDamageHiKey = model;
				data->mToolDamageLoKey = model;
				data->mToolDamageMdKey = model;
				data->mRackModelKey = model;
				data->mName = TribePlanManager.trg_ieventmanager->GetEventItemName(model);
			}
			return data;
		}

		return original_function(toolType);
	}
};

// Detour GetToolClass in cTribeTool
member_detour(GetToolClass_detour, Simulator::cTribeTool, int()) {
	int detoured() {
		if (this->GetToolType() > 13) {
			return 1;
		}
		return original_function(this);
	}
};

// Detour GetRefundMoney in cTribeTool
member_detour(GetRefundMoney_detour, Simulator::cTribeTool, int()) {
	int detoured() {
		// sell for full price if under construction
		if (TribeToolManager.IsToolInProgress(this)) {
			return Simulator::GetTribeToolData(this->GetToolType())->mPrice;
		}
		if (this->GetToolType() == Decor) {
			return 10;
		}
		return original_function(this);
	}
};

// Detour RemoveTool in cTribe
virtual_detour(RemoveTool_detour, Simulator::cTribe, Simulator::cTribe, void(cTribeTool*)) {
	void detoured(cTribeTool * tribeTool) {
		//tribeTool->mTribe = nullptr;
		//this->mTribeLayout.SetParameters(tribeTool->GetPosition(), 0, tribeTool->GetDirection(), 0);
		//this->SetPosition(tribeTool->GetPosition()); // setting tribe position moves the camera and tool-rotation-point to this spot. fucks up the tribe if remains set when saving.
		//this->slot
		TribeToolManager.RemoveFromProgress(tribeTool);
		original_function(this, tribeTool);

	}
};

// Detour GetRolloverIdForObject in SimulatorRollover
static_detour(GetRolloverIdForObject_detour, UI::SimulatorRolloverID(Simulator::cGameData*)) {
	UI::SimulatorRolloverID detoured(Simulator::cGameData * object) {
		//auto tribetool = object_cast<cTribeTool>(object);

		if (IsCreatureGame()) {
			auto nest = object_cast<cNest>(object);
			// player owned nest
			if (nest && nest->IsPlayerOwned()) {
				return UI::SimulatorRolloverID::FixedObject;
			}
		}
		else if (IsTribeGame()) {
			auto tool = object_cast<cTribeTool>(object);
			// player owned nest
			if (tool) {
				if (tool->GetToolType() == Decor) {
					return UI::SimulatorRolloverID::None;
				}
				else {
					auto meta = TribeToolManager.GetTribeToolMetadata(tool->GetToolType());
					if (meta && meta->mbToolDisableRollover) {
						return UI::SimulatorRolloverID::None;
					}
				}
			}
		}

		auto value = original_function(object);
		return value;
	}
};

// Detour CreateTool in cTribe
member_detour(CreateTool_detour, Simulator::cTribe, cTribeTool* (int)) {
	cTribeTool* detoured(int toolType) {
		auto tool = original_function(this, toolType);
		TribeToolManager.AddedTool(tool, this);
		return tool;
	}
};


// Detour DoAction in cCreatureCitizen
member_detour(CitizenDoAction_detour, Simulator::cCreatureCitizen, void(int, cGameData*, App::Property*)) {
	void detoured(int actionId, cGameData * actionObject, App::Property * property) {
		// try to find the missing action ID names
		if ((actionId > 15 && actionId < 20) || (actionId > 21 && actionId < 24) || actionId > 27) {
			SporeDebugPrint("Citizen action %i taken by creature of tribe %ls", actionId, this->mpOwnerTribe->GetCommunityName().c_str());
		}

		if (this->mpOwnerTribe == GameNounManager.GetPlayerTribe() && actionObject != TribePlanManager.trg_ieventmanager->mpEventItem.get()) {
			TribePlanManager.trg_ieventmanager->RemoveCreatureFromActivators(this);
		}
		original_function(this, actionId, actionObject, property);

	}
};


// Detour GetHandheldItemForTool in cCreatureCitizen
// Use this to suppress the repair tool for creatures investigating event items.
// NOTE: doesnt work for grabbing mallet, suppress the item equipping animation instead
member_detour(GetHandheldItemForTool_detour, Simulator::cCreatureCitizen, int(int)) {
	int detoured(int toolType) {

		// chieftain staff
		if (toolType == kTribeToolTypeChieftain) {
			TribeMemberManager.trg_chiefmanager->AddChiefToQueue(this);
		}
		// if new tool, pull data from tool metadata
		else if (toolType > HomeEnd) { // this->mSpecializedTool != this->mCurrentHandheldItem
			auto meta = TribeToolManager.GetTribeToolMetadata(toolType);
			if (meta) {
				auto handheld_index = meta->mHandHeldIndex;
				TribeToolManager.AddCitizenToolTypeToQueue(toolType, handheld_index);

				return handheld_index;
			}
		}
		return original_function(this, toolType);
	}
};

//-----------------------------------

// Detour GetCachedColorFromId
static_detour(GetCachedColorFromId_detour, const Math::ColorRGB& (uint32_t)) {
	Math::ColorRGB IDColor;
	const Math::ColorRGB& detoured(uint32_t colorId) {
		// If the color ID has only 6 digits, read it as an RGB value.
		if (colorId <= 0x00FFFFFF) {
			IDColor = Color(colorId);
			return IDColor;
		}
		return original_function(colorId);
	}
};

// Detours

member_detour(GetTribeMaxPopulation_detour, Simulator::cTribe, size_t()) {
	size_t detoured() {
		int size = TribePlanManager.GetTribeMaxPopulation(this);
		return size;
	}
};

// Detour the UIEventLog ShowEvent func
member_detour(UIShowEvent_detour, Simulator::cUIEventLog, uint32_t(uint32_t, uint32_t, int, Math::Vector3*, bool, int))
{
	uint32_t detoured(uint32_t instanceID, uint32_t groupID, int int1, Math::Vector3 * vector3, bool dontAllowDuplicates, int int2)
	{
		// If this is a warning notif in tribal stage, do not run the func unless player tribe has a watchtower.
		if (IsTribeGame()) {
			/*
			if (instanceID == id("WildAnimalsRaiding") || instanceID == id("raiderscoming") || instanceID == id("Foodraiderscoming")) {
				// if no watchtower, return before sending the warning.
				if (!GameNounManager.GetPlayerTribe() || !GameNounManager.GetPlayerTribe()->GetToolByType(cTribePlanManager::ToolTypes::Watchtower)) {
					return 0x0;
				}
			}*/
			// Scavenger Raid
			if (instanceID == id("WildAnimalsRaiding")) {
				if (TribePlanManager.TribeHasScarecrow(GameNounManager.GetPlayerTribe())) {
					TribePlanManager.trg_suppressscavenger->SuppressScavenger();
					return 0x0;
				}
				auto value = original_function(this, instanceID, groupID, int1, vector3, dontAllowDuplicates, int2);
				return value;
			}
			// Baby Grow Up
			else if (instanceID == id("babygrewup")) {
				auto tribe = GameNounManager.GetPlayerTribe();
				if (tribe) {
					auto member = TribeMemberManager.GetGrownBaby();
					if (member) {
						// either assign a new personality or apply the old one.
						auto personality = TribeMemberManager.GetPersonality(member);
						if (!personality.valid) {
							TribeMemberManager.AssignPersonality(member);
						}
						else {
							TribeMemberManager.ApplyPersonality(personality);
						}
					}
					if (TribeMemberManager.mbSuppressBabyGrowFX) {
						if (member) {
							member->SetIsRolledOver(false);
						}
						return 0x0;
					}
				}
			}

		}
		// fire OG func
		auto value = original_function(this, instanceID, groupID, int1, vector3, dontAllowDuplicates, int2);

		// New city has appeared in civ
		if (IsCivGame() && instanceID == id("NewCityAppears")) {
			//MessageManager.MessageSend(id("NewCityAppeared"), nullptr);
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

		// detect if a baby is growing up and send a message or suppress FX.
		if (instanceId == 0x3A616FEE) {
			if (IsTribeGame() && TribeMemberManager.mbSuppressBabyGrowFX) {
				instanceId = 0x0;
			}
			MessageManager.MessageSend(id("BabyGrowUp"), nullptr);
		}

		if (IsTribeGame()) {
			uint32_t staff_id = TribeMemberManager.trg_chiefmanager->ConvertStaffEffectID(instanceId);
			if (staff_id != 0x0) {
				return original_function(this, staff_id, groupId);
			}
			else {
				uint32_t tool_id = TribeToolManager.ConvertToolEffectID(instanceId);
				return original_function(this, tool_id, groupId);
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
	struct cCollectableItemID detoured(UnkHashMap& unk1, bool firstCall, int unk3, int unlockLevel)
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

bool IsAttackAbility(cCreatureAbilityPtr ability) {
	// is an attack ability
	if (ability->mCategory == 1 && ability->mVerbIconGameLayout.instanceID == 0xAE2CBB58) {
		return true;
	}
	return false;
}

bool IsAbilityInRange(cCreatureCitizenPtr caller, cCombatantPtr target, cCreatureAbilityPtr ability) {
	float distance = Math::distance(target->ToSpatialObject()->GetPosition(), caller->ToSpatialObject()->GetPosition());
	auto radiuscaller = caller->ToSpatialObject()->GetBoundingRadius() * 0.9f;
	auto radiustarget = target->ToSpatialObject()->GetBoundingRadius() * 0.9f;
	if (distance >= ability->mRangeMin && distance <= (ability->mRange + radiuscaller + radiustarget)) {
		return true;
	}
	

	return false;
}

virtual_detour(PlayAbility_detour, Simulator::cCreatureCitizen, Simulator::cCreatureBase, void(int, Anim::AnimIndex*))
{

	void detoured(int abilityIndex, Anim::AnimIndex * dstAnimIndex)
	{
		int index = abilityIndex;
		// if using a new tool, only allow this creature to use this ability.
		
		if (this->mSpecializedTool > HomeEnd) {
			auto tooldata = TribeToolManager.GetTribeToolData(this->mSpecializedTool);
			if (tooldata) {
				
				cCreatureAbility* ref_ability = this->GetAbility(abilityIndex);

				// only override if in range and not in cooldown
				if (IsAttackAbility(ref_ability)) {
					cCreatureAbility* ability = tooldata->mToolAbility.get();
					if (ability && IsAbilityInRange(this, this->mpCombatantTarget, ref_ability)) {

						// Thank you VanillaCold!
						auto meta = TribeToolManager.GetTribeToolMetadata(this->mSpecializedTool);
						bool abilityowned = false;
						for (int i = this->GetAbilitiesCount() - 1; i >= 0; i--)
						{
							if (this->mpSpeciesProfile->mAbilities[i]->mpPropList->GetResourceKey().instanceID == meta->mAbilityKey.instanceID)
							{
								index = i;
								abilityowned = true;
								break;
							}
						}
						if (!abilityowned)
						{
							// add the new ability to the species profile
							this->mpSpeciesProfile->mAbilities.push_back(ability);
							index = this->mpSpeciesProfile->mAbilities.size() - 1;
							this->mRechargingAbilityBits[index] = 0;
							this->mInUseAbilityBits[index] = 0;

							// AbilityState
							auto a = Simulator::cAbilityState(this->mAbilityStates[index - 1]);
							a.field_08 = ResourceID(this->mAbilityStates[index - 1].field_08);
							a.field_08.instanceID = meta->mAbilityKey.instanceID;
							a.field_08.groupID = meta->mAbilityKey.groupID;
							this->mAbilityStates.push_back(a);
						}
						// still recharging, revert to default.
						if (this->mRechargingAbilityBits[index]) {
							index = abilityIndex;
						}
					}
				}
			}
		}

		original_function(this, index, dstAnimIndex);
	}
};

/*
virtual_detour(GetAbility_detour, Simulator::cCreatureCitizen, Simulator::cCreatureBase, cCreatureAbility* (int))
{
	cCreatureAbility* detoured(int index)
	{
		// if using a new tool, only allow this creature to use this ability.
		if (this->mSpecializedTool > HomeEnd) {
			
		}
		return original_function(this, index);
	}
};

virtual_detour(CitizenUpdate_detour, Simulator::cCreatureCitizen, Simulator::cCreatureBase, void (int))
{
	void detoured(int deltaTime)
	{
		original_function(this, deltaTime);
		if (this->mSpecializedTool > HomeEnd) {
			TribeToolManager.SetDesiredHandheldItem(this);
		}
	}
};*/



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

		

		auto value = original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);


		// prevent friendly fights from doing damage between citizens.
		// TODO: prevent these fights entirely.
		if (IsTribeGame()) {
			if (attackerPoliticalID == this->GetPoliticalID()) {
				damage = 0;
			}
		}


		return value;
	}
};


// Whether a cell should attack another cell
static_detour(CellShouldNotAttack_detour, bool(cCellObjectData*, cCellObjectData*))
{
	bool detoured(cCellObjectData* cell1, cCellObjectData* cell2)
	{
		if (!cellcontroller->mbStateStealthed) {
			return original_function(cell1, cell2);
		}
		else if (cell2 && cell2 == Simulator::Cell::GetPlayerCell()) {
			return true;
		}
		return original_function(cell1, cell2);
	}
};


bool sporepediapause = false;

// Detour the audio playing func
static_detour(PlayAudio_detour, void(uint32_t, Audio::AudioTrack)) {
	void detoured(uint32_t soundID, Audio::AudioTrack track) {
		original_function(soundID, track);
		if (soundID == id("spd_exit") || soundID == id("spd_makenew_click") || soundID == id("spd_verb_edit_click")) {
			if (sporepediapause) {
				sporepediapause = false;
				GameTimeManager.Resume(TimeManagerPause::Gameplay);
				App::ScheduleTask(this, &PlayAudio_detour::SendSporepediaClosedMessage, 0.0001f);
			}
		}
	}
	void SendSporepediaClosedMessage() {
		MessageManager.MessageSend(id("SporepediaClosed"), nullptr);
	}
};

static_detour(AssetBrowserShow_Detour, void(ShopperRequest&)) {
	void detoured(ShopperRequest & request) {
		original_function(request);
		// pause game. this will be unpaused later by the sound playing
		if (!sporepediapause) {
			sporepediapause = true;
			GameTimeManager.Pause(TimeManagerPause::Gameplay);
		}
		
	}
};

// Spui loading/spawning detour
member_detour(ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey& name, bool arg_4, uint32_t arg_8) {
		

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
		if (IsTribeGame()) {

			// rolling over a tribe hut that is actually a rare item
			if (name.instanceID == id("Rollover_TribeHut") && TribePlanManager.trg_ieventmanager->IsEventItemHovered())
			{
				ResourceKey newSpui = ResourceKey(id("Rollover_TribeRare"), name.typeID, name.groupID);
				return original_function(this, newSpui, arg_4, arg_8);
			}
			// rolling over a tribe tool
			else if (name.instanceID == id("Rollover_TribeTool") && TribeToolManager.IsConstructionPlotHovered()) {
				ResourceKey newSpui = ResourceKey(id("Rollover_TribeTool_Construction"), name.typeID, name.groupID);
				return original_function(this, newSpui, arg_4, arg_8);
			}
			// rolling over a chieftain
			else if (name.instanceID == id("Rollover_TribeCitizen") && GameViewManager.mHoveredObject) {
				auto citizen = object_cast<cCreatureCitizen>(GameViewManager.mHoveredObject);
				if (citizen) {
					if (citizen->mSpecializedTool == 11) {
						ResourceKey newSpui = ResourceKey(id("Rollover_TribeChieftain"), name.typeID, name.groupID);
						return original_function(this, newSpui, arg_4, arg_8);
					}
					// didgeridoos need longer rollover UI
					else if (citizen->mSpecializedTool == 6) {
						ResourceKey newSpui = ResourceKey(id("Rollover_TribeCitizen_Long"), name.typeID, name.groupID);
						return original_function(this, newSpui, arg_4, arg_8);
					}
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


// adding window to another window
virtual_detour(AddWindow_detour, UTFWin::Window, UTFWin::Window, void(IWindow*)) {
	void detoured(IWindow * pWindow) {
		original_function(this, pWindow);
	}
};


static_detour(ImageSetBackgroundByKey_detour, bool(IWindow*, const ResourceKey&, int imageIndex)) {
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

member_detour(PlayModeSetWindowVisible_detour, Editors::PlayModeUI, void(uint32_t, bool)) {
	void detoured(uint32_t controlID, bool visible) {
		original_function(this, controlID, visible);
		/*
		if (controlID) {
			auto parent = WindowManager.GetMainWindow()->FindWindowByID(0x0445A468);
			UILayout buttonloadcrt;
				
			if (parent && !parent->FindWindowByID(0x41F745F1)) {
				buttonloadcrt.LoadByID(id("editor_button_loadcreature"));
				parent->AddWindow(buttonloadcrt.FindWindowByID(0x0445A469));
			}
		}*/
	}
};

//-----------------------------------
//// EDITOR LOADING

using namespace Editors;
int trg_last_category = 0;
bool trg_has_set_category = false;

// Editor parts palette loading func, PaletteUI::Load
member_detour(PaletteUILoad_detour, Palettes::PaletteUI, void(Palettes::PaletteMain*, UTFWin::IWindow*, bool, Palettes::PaletteInfo*)) {
	void detoured(Palettes::PaletteMain* pPalette, UTFWin::IWindow* pWindow, bool bool1, Palettes::PaletteInfo* pInfo) {
		trg_has_set_category = false;
		original_function(this, pPalette, pWindow, bool1, pInfo);


		// Tribal
		if (IsTribeGame()) {
			// Add rename UI to planner
			TribePlanManager.trg_hutmanager->AddTribeRenameUI(bool(this));
		}
		else {
			trg_has_set_category = false;
		}
	}
};

// PaletteUI::SetActiveCategory
member_detour(PaletteUISetActiveCategory_detour, Palettes::PaletteUI, void(int)) {
	void detoured(int categoryIndex) {

		// Civ
		if (IsCivGame()) {
			// TODO: start tracking if the player mouses over city wall up/downgrades
			cvg_citywalls->ProcessCityWallReduction();
		}

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
			//if (trg_last_category != 0) {
			original_function(this, trg_last_category);
			//}
			Simulator::ScheduleTask(this, &PaletteUISetActiveCategory_detour::EnableHasSetCategory, 0.0001f);
		}

	}
	void PaletteUISetActiveCategory_detour::EnableHasSetCategory() {
		trg_has_set_category = true;
		MessageManager.MessageSend(id("UpdateHut"), nullptr);
		MessageManager.MessageSend(id("UpdateHomes"), nullptr);

		MessageManager.MessageSend(id("UpdateStaffIcon"), nullptr);

	}
};



virtual_detour(EditorOnEnter_detour, cEditor, cEditor, bool()) {
	bool detoured() {
		auto value = original_function(this);
		//if (this->mEditorName == id(0x465C50BA))
		App::ScheduleTask(crg_partshards, &CRG_PartShards::SuppressDestroyedParts, 0.01f);
		return value;
	}
};

// Specialized citizen role names

using Fixed32StringType = eastl::fixed_string<char16_t, 32>;
static_detour(CitizenGetSpecializedName_detour, Fixed32StringType* (Fixed32StringType*, cGameData*)) {
	Fixed32StringType* detoured(Fixed32StringType* type, cGameData* object) {

		auto citizen = object_cast<cCreatureCitizen>(object);
		// only run on new-tool-holders and non-player tribe chieftains
		if (citizen && citizen != last_named_citizen && (citizen->mSpecializedTool > HomeEnd || (citizen->mSpecializedTool == 11 && citizen->mpOwnerTribe != GameNounManager.GetPlayerTribe()))) {
			// store the citizen to use in the text localization
			last_named_citizen = citizen;

		}

		return original_function(type, object);
	}
};

enum TribeRoleNames {
	Chieftain = 0x04bd540b,
	RoleStart = 0x0567d1d0,
	RoleEnd = 0x0567d1d8,
};

//detour settext in LocalizedString
member_detour(LocalStringSetText_detour, LocalizedString, bool(uint32_t, uint32_t, const char16_t*)) {
	bool detoured(uint32_t tableID, uint32_t instanceID, const char16_t* pPlaceholderText) {
		// "Chieftain X"
		if (tableID == 0xF71FA311 && last_named_citizen) {
			if ((instanceID >= RoleStart && instanceID <= 0x0567d1d8)) { // instanceID == Chieftain || 
				auto res = TribeToolManager.GetCitizenNameLocaleResource(last_named_citizen);
				if (res.instanceID != 0x0 && res.groupID != 0x0) {
					tableID = res.groupID;
					instanceID = res.instanceID;
				}
				last_named_citizen = nullptr;
			}
		}
		return original_function(this, tableID, instanceID, pPlaceholderText);
	}
};

//-----------------------------------
// Civ

#include <Spore\Simulator\SubSystem\CommManager.h>
member_detour(ShowCommEvent_detour, Simulator::cCommManager, void(cCommEvent*)) {
	void detoured(cCommEvent* pEvent) {
		if (!IsCommScreenActive()) {

			MessageManager.MessageSend(id("CinematicBegin"), nullptr);
			//GameTimeManager.SetSpeedFactors(1.0f, 2.0f, 4.0f, 8.0f);
			//GameTimeManager.SetSpeed(0);
			pLastEvent = pEvent;
			App::ScheduleTask(this, &ShowCommEvent_detour::ShowLastCommEvent, 0.001f);
		}
		else {
			original_function(this, pEvent);
		}
	}

	void ShowCommEvent_detour::ShowLastCommEvent() {
		original_function(this, pLastEvent);
		pLastEvent = nullptr;
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

static_detour(CityProcessBuildingUpdate_detour, void(cCity*, int, int)) {
	void detoured(cCity * city, int int1, int int2) {
		original_function(city, int1, int2);
		cvg_citywalls->UpdateCityBuildingColor(city);
	}

};


//-----------------------------------


void AttachDetours()
{
	
	// Standalone
	PlayAudio_detour::attach(GetAddress(Audio, PlayAudio));
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));
	EffectOverride_detour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));
	SetModel_detour::attach(GetAddress(Simulator::cSpatialObject, SetModelKey));  

	TribeSpawn_detour::attach(GetAddress(Simulator, SpawnNpcTribe));
	HerdSpawn_detour::attach(GetAddress(Simulator::cGameNounManager, CreateHerd));
	TribeSpawnMember_detour::attach(GetAddress(Simulator::cTribe, SpawnMember));

	AvatarScaling_detour::attach(GetAddress(Simulator::cCreatureGameData, CalculateAvatarNormalizingScale));

	WalkTo_detour::attach(GetAddress(Simulator::cCreatureBase, WalkTo));
	OnJumpLand_detour::attach(GetAddress(Simulator::cCreatureAnimal, OnJumpLand));

	// Abilities
	PlayAbility_detour::attach(GetAddress(Simulator::cCreatureBase, PlayAbility));
	// TODO: needs disk spore address!
	//GetAbility_detour::attach(Address(0x00c047d0));
	//CitizenUpdate_detour::attach(GetAddress(Simulator::cCreatureBase, Update));

	ScenarioPlayModeUpdateGoals_detour::attach(GetAddress(Simulator::cScenarioPlayMode, UpdateGoals));

	// Tribe Tools
	GetTribeToolData_detour::attach(GetAddress(Simulator, GetTribeToolData));
	GetToolClass_detour::attach(GetAddress(Simulator::cTribeTool, GetToolClass));
	GetRefundMoney_detour::attach(GetAddress(Simulator::cTribeTool, GetToolClass));
	RemoveTool_detour::attach(GetAddress(Simulator::cTribe, RemoveTool));
	GetRolloverIdForObject_detour::attach(GetAddress(UI::SimulatorRollover, GetRolloverIdForObject));
	CreateTool_detour::attach(GetAddress(Simulator::cTribe, CreateTool));
	CitizenDoAction_detour::attach(GetAddress(Simulator::cCreatureCitizen, DoAction));
	GetHandheldItemForTool_detour::attach(GetAddress(Simulator::cCreatureCitizen, GetHandheldItemForTool));


	CombatTakeDamage_detour::attach(GetAddress(Simulator::cCombatant, TakeDamage));

	CellShouldNotAttack_detour::attach(GetAddress(Simulator::Cell, ShouldNotAttack));

	AssetBrowserShow_Detour::attach(GetAddress(ShopperRequest, Show));

	ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));
	//AddWindow_detour::attach(GetAddress(UTFWin::Window, AddWindow));
	ImageSetBackgroundByKey_detour::attach(GetAddress(UTFWin::Image, SetBackgroundByKey));
	PaletteUILoad_detour::attach(GetAddress(Palettes::PaletteUI, Load));
	PaletteUISetActiveCategory_detour::attach(GetAddress(Palettes::PaletteUI, SetActiveCategory));
	EditorOnEnter_detour::attach(GetAddress(cEditor, OnEnter));
	PlayModeSetWindowVisible_detour::attach(GetAddress(PlayModeUI, SetWindowVisible));

	GetCachedColorFromId_detour::attach(GetAddress(Simulator, GetCachedColorFromId));

	GetTribeMaxPopulation_detour::attach(Address(0x00c8ec70));
	UIShowEvent_detour::attach(GetAddress(Simulator::cUIEventLog, ShowEvent));

	
	// Creature Names
	CitizenGetSpecializedName_detour::attach(GetAddress(Simulator::cCreatureCitizen, GetSpecializedName));
	LocalStringSetText_detour::attach(GetAddress(LocalizedString, SetText));
	
	// Civ
	ShowCommEvent_detour::attach(GetAddress(Simulator::cCommManager, ShowCommEvent));
	CitySpawnVehicle_detour::attach(GetAddress(Simulator::cCity, SpawnVehicle));
	CityProcessBuildingUpdate_detour::attach(GetAddress(Simulator::cCity, ProcessBuildingUpdate));

	CRGunlock_detour::attach(GetAddress(Simulator::CreatureGamePartUnlocking, sub_D3B460));
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

