#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"
// CVG
#include "TribeMemberManager.h"
#include "TribePlanManager.h"
#include "TribeToolStratManager.h"
#include "TribeToolManager.h"
#include "cCreaturePickup.h"

#define HBTribePtr intrusive_ptr<HBTribe>
using namespace Simulator;
using namespace Editors;

cCreatureCitizenPtr last_named_citizen;

// TRG

class HBTribe 
{
public:
	
	static void Initialize() {
		auto trg_membermanager = new(cTribeMemberManager);
		auto trg_tribeplanmanager = new(cTribePlanManager);
		auto trg_toolstratmanager = new(cTribeToolStratManager);
		auto trg_toolmanager = new(cTribeToolManager);

		auto creaturepickup = new(cCreaturePickup);
	}

	static void Dispose() {
		last_named_citizen = nullptr;
	}

	static void AttachDetours();

};

//----------
// Detours

bool TRG_ReadSPUI_detour(UTFWin::UILayout* obj, ResourceKey& name, bool& arg_4, uint32_t& arg_8) {
	if (IsTribeGame()) {

		// rolling over a tribe hut that is actually a rare item
		if (name.instanceID == id("Rollover_TribeHut") && TribePlanManager.trg_ieventmanager->IsEventItemHovered())
		{
			name = ResourceKey(id("Rollover_TribeRare"), name.typeID, name.groupID);
			return true;
		}
		// rolling over a tribe tool
		else if (name.instanceID == id("Rollover_TribeTool") && TribeToolManager.IsConstructionPlotHovered()) {
			name = ResourceKey(id("Rollover_TribeTool_Construction"), name.typeID, name.groupID);
			return true;
		}
		// rolling over a chieftain
		else if (name.instanceID == id("Rollover_TribeCitizen") && GameViewManager.mHoveredObject) {
			auto citizen = object_cast<cCreatureCitizen>(GameViewManager.mHoveredObject);
			if (citizen) {
				if (citizen->mSpecializedTool == 11) {
					name = ResourceKey(id("Rollover_TribeChieftain"), name.typeID, name.groupID);
					return true;
				}
				// didgeridoos need longer rollover UI
				else if (citizen->mSpecializedTool == 6) {
					name = ResourceKey(id("Rollover_TribeCitizen_Long"), name.typeID, name.groupID);
					return true;
				}
			}

		}

	}
}

// Spui loading/spawning detour
member_detour(TRG_ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey & name, bool arg_4, uint32_t arg_8) {
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

		return original_function(this, name, arg_4, arg_8);
	}
};

// Detour the playanimation ID-picking func
member_detour(TRG_AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

		if (IsTribeGame()) {

			// Getting tool from rack
			if (animID == 0x03CDACC0) { // get_tool_rack
				auto creature = Common::GetAnimCreatureCitizenOwner(this);

				// if the tool has a custom tool interaction anim, use that instead of get_tool_rack.
				int tooltype = TribeMemberManager.GetCreatureDesiredTool(creature);
				if (tooltype > -1) {
					uint32_t interact_anim = TribeToolStratManager.GetCreatureInteractAnim(creature, tooltype);
					if (interact_anim != 0x0) { animID = interact_anim; }

					TribeMemberManager.SetCreatureDesiredToolGrabState(creature, true);
				}

				creature->mCurrentHandheldItem = kHandheldItemCityProtestSignHunger;
			}
			// Getting/putting away tool from inventory
			else if (animID == 0x02C39200) { // get_tool
				bool suppress = false;
				auto creature = Common::GetAnimCreatureCitizenOwner(this);

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

					if (!suppress) {
						// the city protest sign is not used in tribal, so it can be used as a dummy item to ensure the value will always have to change, even if going to 0.
						creature->mCurrentHandheldItem = kHandheldItemCityProtestSignHunger;
					}
				}
				if (suppress) {
					return original_function(this, 0x0, pChoice);
				}
			}
			// En Route to attack
			// TODO: fix tool disappearing when attacking buildings!
			else if (animID >= 0x0004CFFB && animID <= 0x0004CFFD) {
				auto creature = Common::GetAnimCreatureCitizenOwner(this);
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
					auto creature = Common::GetAnimCreatureCitizenOwner(this);
					if (creature && creature == GameNounManager.GetPlayerTribe()->GetLeaderCitizen()) {

						return original_function(this, 0x056D2CF2, pChoice); // axe_attack

					}
				}
			}
			// Chieftain fishing
			else if ((animID >= 0x02C39231 && animID <= 0x02C39244) || animID == 0x05f08fa3) {
				if (TribeMemberManager.trg_chiefmanager->mbStaffSingleSided) {
					auto creature = Common::GetAnimCreatureCitizenOwner(this);
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

			// tribe animal finished eating bone treat
			else if (animID == 0xD20446B0) { // mot_carcass_eat_stop 
				auto creature = Common::GetAnimCreatureOwner(this);

				// restore health and hunger
				creature->SetHealthPoints(creature->GetMaxHitPoints());
				creature->mHunger = 100.0f;
				// make relationship restore, if negative
				// TODO: not working.
				//RelationshipManager.ApplyRelationship(creature->mHerd->mPoliticalID, GameNounManager.GetPlayerTribe()->GetPoliticalID(), kRelationshipEventTribeGift);
			}

			// Marshmallow
			else if (animID == 0x06426C10) { // vig_roastmarshmallow
				auto creature = Common::GetAnimCreatureCitizenOwner(this);
				if (creature) {

					// randomize whether to use an alternate roast anim
					if (randf() >= 0.5) {
						// TODO: Fixme!
						int herb = CapabilityChecker::GetCapabilityLevel(creature, 0x022E785C);
						if (herb > 0 /*creature->IsHervibore()*/) {
							int carn = CapabilityChecker::GetCapabilityLevel(creature, 0x022E7847);

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
				auto creature = Common::GetAnimCreatureCitizenOwner(this);
				if (creature) {
					TribePlanManager.trg_firedancemanager->AddDancer(creature);
				}
			}
			else if (animID == TRG_FireDanceManager::DanceAnims::soc_celebrate_trg) {
				auto creature = Common::GetAnimCreatureCitizenOwner(this);
				if (creature) {
					TribePlanManager.trg_firedancemanager->HitLastDanceAnim(creature);
				}
			}

		}

		// Jump fix
		if (IsTribeGame() || IsScenarioMode()) {
			if (animID == 0x05261D56) { // gen_hover_jump
				auto creature = Common::GetAnimCreatureCitizenOwner(this);
				if (CapabilityChecker::GetCapabilityLevel(creature, 0x04F4E1B4) == 0) { //modelCapabilityGlide
					return original_function(this, 0x025B3BC0, pChoice);
				}
			}
		}

		return original_function(this, animID, pChoice);

	}
};

// Detour the model setting func
// TODO: move to the tool manager?
virtual_detour(TRG_SetModel_detour, cSpatialObject, cSpatialObject, void(const ResourceKey&)) {
	void detoured(const ResourceKey & modelKey) {

		// For updating the tribal hut models while they are build
		if (IsTribeGame()) {
			auto tribetool = object_cast<cTribeTool>(this);
			if (tribetool && TribeToolManager.IsToolInProgress(tribetool)) {
				auto model = TribeToolManager.GetPlotModel(tribetool);
				original_function(this, model);
				return;
			}
		}

		original_function(this, modelKey);
	}
};

// Detour ChooseInputActionID in cTribeInputStrategy
// TODO: NOT CURRENTLY USED
member_detour(TRG_ChooseInputActionID_detour, cTribeInputStrategy, int(cGameData*, int)) {
	int detoured(cGameData* object, int unk) {

		int value = original_function(this, object, unk);

		return value;
	}
};

//-----------------------------------
//// TRIBE CITIZEN NAMES DETOURS

// Specialized citizen role names

using Fixed32StringType = eastl::fixed_string<char16_t, 32>;
static_detour(CitizenGetSpecializedName_detour, Fixed32StringType* (Fixed32StringType*, cGameData*)) {
	Fixed32StringType* detoured(Fixed32StringType * type, cGameData * object) {

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
//// TRIBE TOOLS DETOURS

// Detour GetTribeToolData in cTribeToolData
static_detour(TRG_GetTribeToolData_detour, cTribeToolData* (int)) {
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
member_detour(TRG_GetToolClass_detour, cTribeTool, int()) {
	int detoured() {
		if (this->GetToolType() > 13) {
			return 1;
		}
		return original_function(this);
	}
};

// Detour GetRefundMoney in cTribeTool
member_detour(TRG_GetRefundMoney_detour, cTribeTool, int()) {
	int detoured() {
		// sell for full price if under construction
		if (TribeToolManager.IsToolInProgress(this)) {
			return GetTribeToolData(this->GetToolType())->mPrice;
		}
		if (this->GetToolType() == Decor) {
			return 10;
		}
		return original_function(this);
	}
};

// Detour RemoveTool in cTribe
virtual_detour(TRG_RemoveTool_detour, cTribe, cTribe, void(cTribeTool*)) {
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
static_detour(TRG_GetRolloverIdForObject_detour, UI::SimulatorRolloverID(cGameData*)) {
	UI::SimulatorRolloverID detoured(cGameData * object) {
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
member_detour(TRG_CreateTool_detour, cTribe, cTribeTool* (int)) {
	cTribeTool* detoured(int toolType) {
		auto tool = original_function(this, toolType);
		TribeToolManager.AddedTool(tool, this);
		return tool;
	}
};


// Detour DoAction in cCreatureCitizen
member_detour(TRG_CitizenDoAction_detour, cCreatureCitizen, void(int, cGameData*, App::Property*)) {
	void detoured(int actionId, cGameData * actionObject, App::Property * property) {

		// try to find the missing action ID names
		if ((actionId > 15 && actionId < 20) || (actionId > 21 && actionId < 24) || actionId > 28) {
			SporeDebugPrint("Citizen action %i taken by creature of tribe %ls", actionId, this->mpOwnerTribe->GetCommunityName().c_str());
		}

		// citizen is getting a tool from a shed. Store the creature and tool ID if it is new, and erase if not getting a tool or getting a default tool.
		if (actionId == kCitizenActionGrabTool) {
			auto tool = object_cast<cTribeTool>(actionObject);
			if (tool && tool->GetToolType() >= HomeStart) {
				TribeMemberManager.StoreCreatureDesiredTool(this, tool->GetToolType());
			}
			else {
				TribeMemberManager.RemoveCreatureDesiredTool(this);
			}
		}
		else {
			TribeMemberManager.RemoveCreatureDesiredTool(this);
		}


		if (this->mpOwnerTribe == GameNounManager.GetPlayerTribe() && actionObject != TribePlanManager.trg_ieventmanager->mpEventItem.get()) {
			TribePlanManager.trg_ieventmanager->RemoveCreatureFromActivators(this);
		}
		original_function(this, actionId, actionObject, property);

	}
};



// Detour GetHandheldItemForTool in cCreatureCitizen
member_detour(TRG_GetHandheldItemForTool_detour, cCreatureCitizen, int(int)) {
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

				// only fire these if the creature's tool is in the queue, meaning it is from a toolshed, and if it is set that the creature has begun to grab it.
				if (TribeMemberManager.GetCreatureDesiredTool(this, true) > -1) {
					TribeToolStratManager.CreatureAcquiredTool(this, toolType);
					TribeMemberManager.RemoveCreatureDesiredTool(this);
				}
				// fire this if the creature's tool is already owned
				else {
					TribeToolStratManager.CreatureSwitchedToTool(this, toolType);
				}

				if (handheld_index == 0) {
					this->mSpecializedTool = 0;
				}

				return handheld_index;
			}
		}
		return original_function(this, toolType);
	}
};

// Detour the effect playing func
member_detour(TRG_EffectOverride_detour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId) // Detour the function for obtaining effect indexes
	{
		if (IsTribeGame()) {
			// detect if a baby is growing up and send a message or suppress FX.
			if (instanceId == 0x3A616FEE) {
				if (TribeMemberManager.mbSuppressBabyGrowFX) {
					instanceId = 0x0;
				}
			}

			uint32_t staff_id = TribeMemberManager.trg_chiefmanager->ConvertStaffEffectID(instanceId);
			if (staff_id != 0x0) {
				return original_function(this, staff_id, groupId);
			}
			else {
				uint32_t tool_id = TribeToolManager.ConvertToolEffectID(instanceId);
				return original_function(this, tool_id, groupId);
			}

		}

		return original_function(this, instanceId, groupId); // Call the original function with the new instance ID.
	}
};

virtual_detour(TRG_PlayAbility_detour, cCreatureCitizen, cCreatureBase, void(int, Anim::AnimIndex*))
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
				if (Common::IsAttackAbility(ref_ability)) {
					cCreatureAbility* ability = tooldata->mToolAbility.get();
					if (ability && Common::IsAbilityInRange(this, this->mpCombatantTarget, ref_ability)) {

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
							auto a = cAbilityState(this->mAbilityStates[index - 1]);
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


// Detour the tribe member spawning func
member_detour(TRG_TribeSpawnMember_detour, cTribe, cCreatureCitizen* (int)) {
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


member_detour(TRG_GetTribeMaxPopulation_detour, cTribe, size_t()) {
	size_t detoured() {
		int size = TribePlanManager.GetTribeMaxPopulation(this);
		return size;
	}
};

// Detour the UIEventLog ShowEvent func
member_detour(TRG_UIShowEvent_detour, cUIEventLog, uint32_t(uint32_t, uint32_t, int, Math::Vector3*, bool, int))
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

		return value;
	}
};

// Detour the cursor setting func
// TODO: Move parts of this detour to cCreaturePickup
member_detour(TRG_SetCursor_detour, UTFWin::cCursorManager, bool(uint32_t)) {
	bool detoured(uint32_t id) {

		if (IsTribeGame()) {

			//// Only run these if not in the planner
			// ---------------------------------------
			if (!Common::IsPlannerOpen()) {
				// held tribe member
				if (CreaturePickupManager.IsCreatureHeld()) {
					return original_function(this, 0x03C32077);
				}
				else {
					// check if a member of the tribe is hovered over
					auto tribe = CreaturePickupManager.GetPlayerTribe();
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

// Called when a combatant takes damage
virtual_detour(TRG_CombatTakeDamage_detour, cCombatant, cCombatant, int(float, uint32_t, int, const Vector3&, cCombatant*))
{
	int detoured(float damage, uint32_t attackerPoliticalID, int integer, const Vector3 & vector, cCombatant * pAttacker)
	{
		// Prevent friendly fights from doing damage between citizens.
		// TODO: prevent these fights entirely.
		if (IsTribeGame()) {
			if (attackerPoliticalID == this->GetPoliticalID()) {
				damage = 0;
			}
		}
		return original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);
	}
};

int trg_last_category = 0;
bool trg_has_set_category = false;

// Editor parts palette loading func, PaletteUI::Load
member_detour(TRG_PaletteUILoad_detour, Palettes::PaletteUI, void(Palettes::PaletteMain*, UTFWin::IWindow*, bool, Palettes::PaletteInfo*)) {
	void detoured(Palettes::PaletteMain * pPalette, UTFWin::IWindow * pWindow, bool bool1, Palettes::PaletteInfo * pInfo) {
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
member_detour(TRG_PaletteUISetActiveCategory_detour, Palettes::PaletteUI, void(int)) {
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
			//if (trg_last_category != 0) {
			original_function(this, trg_last_category);
			//}
			Simulator::ScheduleTask(this, &TRG_PaletteUISetActiveCategory_detour::EnableHasSetCategory, 0.0001f);
		}

	}
	void TRG_PaletteUISetActiveCategory_detour::EnableHasSetCategory() {
		trg_has_set_category = true;
		MessageManager.MessageSend(id("UpdateHut"), nullptr);
		MessageManager.MessageSend(id("UpdateHomes"), nullptr);

		MessageManager.MessageSend(id("UpdateStaffIcon"), nullptr);

	}
};

//-----------------
// ATTACH DETOURS
void HBTribe::AttachDetours() {
	cTribeToolManager::AttachDetours();

	TRG_ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));
	TRG_AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	TRG_SetModel_detour::attach(GetAddress(cSpatialObject, SetModelKey));
	// Creature Names
	CitizenGetSpecializedName_detour::attach(GetAddress(cCreatureCitizen, GetSpecializedName));
	LocalStringSetText_detour::attach(GetAddress(LocalizedString, SetText));

	// Tribe Tools
	TRG_GetTribeToolData_detour::attach(GetAddress(Simulator, GetTribeToolData));
	TRG_GetToolClass_detour::attach(GetAddress(cTribeTool, GetToolClass));
	TRG_GetRefundMoney_detour::attach(GetAddress(cTribeTool, GetToolClass));
	TRG_RemoveTool_detour::attach(GetAddress(cTribe, RemoveTool));
	TRG_GetRolloverIdForObject_detour::attach(GetAddress(UI::SimulatorRollover, GetRolloverIdForObject));
	TRG_CreateTool_detour::attach(GetAddress(cTribe, CreateTool));
	TRG_CitizenDoAction_detour::attach(GetAddress(cCreatureCitizen, DoAction));
	TRG_GetHandheldItemForTool_detour::attach(GetAddress(cCreatureCitizen, GetHandheldItemForTool));
	TRG_EffectOverride_detour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));
	//TRG_ChooseInputActionID_detour::attach(GetAddress(cTribeInputStrategy, ChooseInputActionID));
	TRG_PlayAbility_detour::attach(GetAddress(cCreatureBase, PlayAbility));
	

	TRG_TribeSpawnMember_detour::attach(GetAddress(cTribe, SpawnMember));
	TRG_GetTribeMaxPopulation_detour::attach(Address(0x00c8ec70));
	TRG_UIShowEvent_detour::attach(GetAddress(cUIEventLog, ShowEvent));
	TRG_SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));
	TRG_CombatTakeDamage_detour::attach(GetAddress(cCombatant, TakeDamage));

	TRG_PaletteUILoad_detour::attach(GetAddress(Palettes::PaletteUI, Load));
	TRG_PaletteUISetActiveCategory_detour::attach(GetAddress(Palettes::PaletteUI, SetActiveCategory));
}
