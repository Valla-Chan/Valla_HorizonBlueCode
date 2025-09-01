#include "stdafx.h"
#include "devPlayComm.h"
#include <Spore\Simulator\SubSystem\CommManager.h>

using namespace Simulator;

devPlayComm::devPlayComm()
{
}


devPlayComm::~devPlayComm()
{
}


void devPlayComm::ParseLine(const ArgScript::Line& line)
{
	cPlanetRecord* planetRecord = Simulator::GetActivePlanetRecord();
	PlanetID planetID = 0;
	if (planetRecord) {
		planetID = planetRecord->GetID();
	};
	//TODO: missing lots of data
	if (IsSpaceGame()) {
		CommManager.ShowCommEvent(CommManager.CreateSpaceCommEvent(0x0, planetID, id("civ_main_menu"), id("player_contact_npc")));
	}
	else {
		cCivilization* pCiv = GameNounManager.GetPlayerCivilization();
		cCity* city = nullptr;
		if (pCiv && pCiv->mCities.size() > 0) {
			city = pCiv->mCities[0].get();
		}
		CommManager.ShowCommEvent(CommManager.CreateCivCommEvent(pCiv, city, planetID, id("civ_main_menu"), id("player_contact_npc")));
	}
}

const char* devPlayComm::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Plays a communications entry.";
	}
	else {
		return "devPlayComm: Plays a communications entry.";
	}
}
