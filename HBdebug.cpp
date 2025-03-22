#include "stdafx.h"
#include "HBdebug.h"
#include <Spore\Editors\Editor.h>
#include <Spore\Simulator\cCreatureGameData.h>


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
	auto civilization = GameNounManager.GetPlayerCivilization();
	auto cities = GetData<cCity>();
	cCityPtr firstCity; if (!cities.empty()) { firstCity = cities[0]; }
	//-------------------------------------------
	// Your code here:
	//-------------------------------------------

	if (hovered) {
		hovered = hovered;
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
