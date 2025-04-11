#include "stdafx.h"
#include "HBdebug.h"
#include <Spore\Editors\Editor.h>
#include <Spore\Simulator\cCreatureGameData.h>

using namespace Simulator;
using namespace Cell;

HBdebug::HBdebug()
{
}


HBdebug::~HBdebug()
{
}

//-----------------------------------------------------------------------------------

Vector3 camPos = Vector3(0, 0, 0);
Vector3 camDir = Vector3(0, 0, 0);
void TraceHeader() {
	auto pViewer = CameraManager.GetViewer();

	// Get vector to the center of the screen.
	auto windowArea = WindowManager.GetMainWindow()->GetArea();
	pViewer->GetCameraToMouse(camPos, camDir);
}

cCreatureCitizenPtr TraceHitCitizen() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto citizen = object_cast<cCreatureCitizen>(object);
			if (citizen && citizen->mpOwnerCity->IsPlayerOwned()) {
				return citizen;
			}
		}
	}
	return nullptr;
}

cCreatureAnimalPtr TraceHitAnimal() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto animal = object_cast<cCreatureAnimal>(object);
			if (animal) {
				return animal;
			}
		}
	}
	return nullptr;
}

// not currently working
cGamePlantPtr TraceHitPlant() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto plant = object_cast<cGamePlant>(object);
			return plant;
		}
	}
	return nullptr;
}

cSpatialObjectPtr TraceHitSpatial() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			return object;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------

void HBdebug::StoreData() {
// Some common vars we might want to use
//--------------------------------------------------------------------

// Objects and Players
	avatar = GameNounManager.GetAvatar();
	player = GetPlayer();
	/* herd */ if (avatar) { herd = avatar->mHerd; }
	/* nest */ if (herd) { nest = herd->mpNest; }
	tribe = GameNounManager.GetPlayerTribe();
	/* chieftain */ if (tribe) { chieftain = tribe->GetLeaderCitizen(); }
	hovered = GameViewManager.GetHoveredObject();
	hoveredAnimal = object_cast<cCreatureAnimal>(hovered);
	hoveredCitizen = object_cast<cCreatureCitizen>(hovered);
	hoveredCombatant = object_cast<cCombatant>(hovered);
	ship = Simulator::GetPlayerUFO();

// UI
	window = WindowManager.GetMainWindow();
// Editor
	editor = GetEditor();
	editorModel = Editor.GetEditorModel();

// CRG part unlocks
	/* crgparts */ if (player) { crgparts = player->mpCRGItems; }

// Planet
	planetRecord = Simulator::GetActivePlanetRecord();
	empire = Simulator::GetPlayerEmpire();
	civilization = GameNounManager.GetPlayerCivilization();
	cities = GetData<cCity>();
/* firstCity */ if (!cities.empty()) { firstCity = cities[0]; }

}


void HBdebug::ParseLine(const ArgScript::Line& line)
{
	StoreData();
//--------------------------------------------------------------------
// Your code here:
//--------------------------------------------------------------------
	
	auto ufo = Simulator::CreateUFO(Simulator::UfoType::Unk5, empire.get());


//--------------------------------------------------------------------
	SporeDebugPrint("HBdebug done executing.");
}



const char* HBdebug::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Horizon Blue Debugging Placeholder. This cheat does nothing unless modified by a developer.";
	}
	else {
		return "HBdebug: Debugging Placeholder. This cheat does nothing unless modified by a developer.";
	}
}


//-------------------------------------------------------------------------------------------------------------
// Premade Funcs


void HBdebug::TribesAttackShip() {
	for (auto tribeitem : Simulator::GetData<Simulator::cTribe>()) {

		for (auto member : tribeitem->GetTribeMembers()) {
			member->field_1001 = false;
			member->field_1002 = false;

			//member->mSpecializedTool = 3;
			//member->mCurrentHandheldItem = kHandheldItemTrgHuntingTool3;
			/*
			if ( false && member->mSpecializedTool != 11 && member->mSpecializedTool != 3) {

				cTribeToolPtr tool = tribeitem->GetToolByType(3);
				if (tool == nullptr) {
					tool = tribeitem->GetToolByType(2);
				}
				if (tool == nullptr) {
					tool = tribeitem->GetToolByType(1);
				}
				if (tool != nullptr) {
					member->DoAction(kCitizenActionGrabTool, tool.get());
				}


			}
			else {*/
			member->DoAction(kCitizenActionAttack, ship.get());
			member->SetTarget(ship.get());
			member->mpTarget = ship;
			member->mpCombatantTarget = ship.get();

			ResourceKey abilitykey = ResourceKey(id("Adventurer_Missile1"), TypeIDs::Names::prop, id("CreatureAbilities"));

			int abilityindex = -1;
			bool abilityowned = false;
			for (int i = member->mpSpeciesProfile->mAbilities.size() - 1; i >= 0; i--)
			{
				if (member->mpSpeciesProfile->mAbilities[i]->mpPropList->GetResourceKey().instanceID == abilitykey.instanceID)
				{
					abilityindex = i;
					abilityowned = true;
					break;
				}
			}
			if (!abilityowned)
			{
				cCreatureAbility* ability = new(cCreatureAbility);
				PropertyListPtr mpPropList;
				if (PropManager.GetPropertyList(abilitykey.instanceID, abilitykey.groupID, mpPropList))
				{
					cCreatureAbility::Parse(ability, mpPropList.get());

					// add the new ability to the species profile
					member->mpSpeciesProfile->mAbilities.push_back(ability);
					abilityindex = member->mpSpeciesProfile->mAbilities.size() - 1;
					member->mRechargingAbilityBits[abilityindex] = 0;
					member->mInUseAbilityBits[abilityindex] = 0;

					// AbilityState
					auto a = Simulator::cAbilityState(member->mAbilityStates[abilityindex - 1]);
					a.field_08 = ResourceID(member->mAbilityStates[abilityindex - 1].field_08);
					a.field_08.instanceID = abilitykey.instanceID;
					a.field_08.groupID = abilitykey.groupID;
					member->mAbilityStates.push_back(a);

				}

			}
			if (abilityindex > 0) {
				member->PlayAbility(abilityindex);
			}




			//}




		}

	}
}
