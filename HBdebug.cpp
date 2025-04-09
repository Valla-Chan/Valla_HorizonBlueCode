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



void HBdebug::ParseLine(const ArgScript::Line& line)
{
	// Some common vars we might want to use
	//-------------------------------------------
	// objects and players
	auto avatar = GameNounManager.GetAvatar();
	auto player = GetPlayer();
	cHerdPtr herd; if (avatar) { herd = avatar->mHerd; }
	cNestPtr nest; if (herd) { nest = herd->mpNest; }
	auto tribe = GameNounManager.GetPlayerTribe();
	cCreatureCitizenPtr chieftain; if (tribe) { chieftain = tribe->GetLeaderCitizen(); }
	auto hovered = GameViewManager.GetHoveredObject();
	auto hoveredAnimal = object_cast<cCreatureAnimal>(hovered);
	auto hoveredCitizen = object_cast<cCreatureCitizen>(hovered);
	auto hoveredCombatant = object_cast<cCombatant>(hovered);
	// ui
	auto window = WindowManager.GetMainWindow();
	// editor
	auto editor = GetEditor();
	auto editorModel = Editor.GetEditorModel();
	// CRG part unlocks
	cCollectableItemsPtr crgparts; if (player) { crgparts = player->mpCRGItems; }
	// planet
	auto planetRecord = Simulator::GetActivePlanetRecord();
	auto empire = Simulator::GetPlayerEmpire();
	auto ship = Simulator::GetPlayerUFO();
	auto civilization = GameNounManager.GetPlayerCivilization();
	auto cities = GetData<cCity>();
	cCityPtr firstCity; if (!cities.empty()) { firstCity = cities[0]; }
	//-------------------------------------------
	// Your code here:
	//-------------------------------------------
	
	if (hovered) {
		hovered = hovered;
		if (hoveredCitizen) {
			chieftain->DoAction(kCitizenActionGatherMeat, hoveredCitizen);
		}
	}

	if (tribe) {
		//tribe->mTribeLayout.mSlots.clear();
		for (size_t i = 0; i < tribe->mTribeLayout.mSlots.size(); i++) {
			tribe->mTribeLayout.mSlots[i].mPosition = Vector3(0, 0, 0);
		}
		//tribe->mTribeLayout.mSlots.clear();
		//vector<Math::Vector3> positions;
		//tribe->mTribeLayout.InitializeSlots(positions, 1.0);
	}

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
				member->DoAction(kCitizenActionAttack, ship);
				member->SetTarget(ship);
				member->mpTarget = ship;
				member->mpCombatantTarget = ship;

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

	//auto campfire = tribe->GetToolByType(10);
	//if (campfire) {
	//	campfire->SetToolType(1);
	//}

	if (IsCellGame()) {
	
		Simulator::cObjectPoolIndex cellIndices[200];
		cCellObjectData* avatar = GetPlayerCell();

		int numCells = Simulator::Cell::FindCellsInRadius(
			CellGame.mpCellQuery,
			avatar->GetPosition(), 200.0f,  // center and radius of search
			cellIndices, 200);

		// stop all found cells from chasing the player
		for (int i = 0; i < numCells; i++)
		{
			cCellObjectData* cell = CellGame.mCells.Get(cellIndices[i]);
			if (cell->IsCreature()) {
				if (cell->field_354) {
					SporeDebugPrint("field_354 true");
				}
				if (cell->field_110) {
					SporeDebugPrint("field_110 true");
				}
				if (cell->field_112) {
					SporeDebugPrint("field_112 true");
				}
				if (cell->field_113) {
					SporeDebugPrint("field_113 true");
				}
			}
		}

	}

	//if (herd && herd->mpHerdMom) {
	//	herd->mpHerdMom->mbColorIsIdentity = true;
	//	herd->mpHerdMom->GrowUp();
	//	herd->mpHerdMom->SetIdentityColor(ColorRGB(5, 0, 3));
	//}

	//auto key = ResourceKey(0x06577404, TypeIDs::Names::crt, 0x40626200);
	//editor->AddCreature(1, &key);

	//-------------------------------------------
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
