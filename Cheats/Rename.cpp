#include "stdafx.h"
#include "Rename.h"

Rename::Rename()
{
}


Rename::~Rename()
{
}

cCreatureCitizenPtr Rename::TraceHitCitizen() const {

	auto pViewer = CameraManager.GetViewer();

	Vector3 camPos = Vector3(0, 0, 0);
	Vector3 camDir = Vector3(0, 0, 0);

	// Get vector to the center of the screen.
	auto windowArea = WindowManager.GetMainWindow()->GetArea();
	pViewer->GetCameraToMouse(camPos, camDir);

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

void Rename::ParseLine(const ArgScript::Line& line)
{
	auto hovered = GameViewManager.GetHoveredObject();
	mParameter = line.GetArguments(1)[0];
	string16 name = string16(string16::CtorConvert(), mParameter);

	if (hovered) {
		auto creature = object_cast<cCreatureBase>(hovered);
		cCreatureCitizenPtr citizen;
		if (IsCivGame()) {
			citizen = TraceHitCitizen();
		}
		if (creature) {
			creature->mCreatureName = name;
			return;
		}
		else if (citizen) {
			citizen->mCreatureName = name;
			return;
		}
		auto hut = object_cast<cTribeHut>(hovered);
		if (hut) {
			hut->mpTribe->SetName(name.c_str());
			return;
		}
		auto tool = object_cast<cTribeTool>(hovered);
		if (tool) {
			tool->mTribe->SetName(name.c_str());
			return;
		}
		auto building = object_cast<cBuilding>(hovered);
		if (building) {
			building->GetOwnerCity()->SetName(name.c_str());
			return;
		}
		auto turret = object_cast<cTurret>(hovered);
		if (turret) {
			turret->mpCity->SetName(name.c_str());
			return;
		}
	}
	else {
		App::ConsolePrintF("You must hover a creature or community object to use this command.");
	}
}

const char* Rename::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Renames the hovered creature, city, or tribe. To target a city or tribe, hover over a placed building.";
	}
	else {
		return "Rename: Renames the hovered creature, city, or tribe. To target a city or tribe, hover over a placed building.";
	}
}
