#include "stdafx.h"
#include "TRG_GetClosestHerd.h"

TRG_GetClosestHerd::TRG_GetClosestHerd()
{
}


TRG_GetClosestHerd::~TRG_GetClosestHerd()
{
}


void TRG_GetClosestHerd::ParseLine(const ArgScript::Line& line)
{
	auto creatures = Simulator::GetDataByCast<cCreatureAnimal>();
	// Loop through the creature parts to find this capability at its highest level
	for (auto creature: creatures) {
		if (creature->IsRolledOver()) {
			App::ConsolePrintF("Creature herd ID: %x", creature->mHerd->mArchetype);
		}
	}
}

const char* TRG_GetClosestHerd::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Prints the archetype ID of the herd of the hovered creature.";
	}
	else {
		return "GetHerd: Prints the archetype ID of the herd of the hovered creature.";
	}
}
