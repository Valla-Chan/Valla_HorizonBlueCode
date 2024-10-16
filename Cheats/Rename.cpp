#include "stdafx.h"
#include "Rename.h"

Rename::Rename()
{
}


Rename::~Rename()
{
}


void Rename::ParseLine(const ArgScript::Line& line)
{
	auto hovered = GameViewManager.GetHoveredObject();
	mParameter = line.GetArguments(1)[0];
	string16 name = string16(string16::CtorConvert(), mParameter);

	if (hovered) {
		auto creature = object_cast<cCreatureBase>(hovered);
		if (creature) {
			creature->mCreatureName = name;
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
