#include "stdafx.h"
#include "MarkInteractives.h"

MarkInteractives::MarkInteractives()
{
}


MarkInteractives::~MarkInteractives()
{
}


using namespace Simulator;
#include <Spore\Editors\Editor.h>
void MarkInteractives::ParseLine(const ArgScript::Line& line)
{
	for (auto item : Simulator::GetDataByCast<cInteractiveOrnament>()) {
		if (item) {
			item->SetScale(4);
		}
	}

	/*
	for (auto item : Simulator::GetDataByCast<cGameData>()) {
		if (item) {
			item->GetPoliticalID();
		}
	}*/

	for (auto item : Simulator::GetDataByCast<cOrnament>()) {
		if (item) {
			item->SetScale(7);
			item->GetModel()->SetColor(ColorRGBA(1.0f, 0, 0, 1.0f));
		}
	}



}

const char* MarkInteractives::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Make all Scenario objects large.";
	}
	else {
		return "MarkInteractives: Make all Scenario objects large.";
	}
}
