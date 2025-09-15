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
	cStarRecord* starRecord = planetRecord->GetStarRecord();
	PlanetID planetID = 0;
	if (planetRecord) {
		planetID = planetRecord->GetID();
	};
	//TODO: missing lots of data
	if (IsCivGame()) {
		cCivilization* pCiv = GameNounManager.GetPlayerCivilization();
		cCity* city = nullptr;
		if (pCiv && pCiv->mCities.size() > 0) {
			city = pCiv->mCities[0].get();
		}
		CommManager.ShowCommEvent(CommManager.CreateCivCommEvent(pCiv, city, planetID, 0xc8fbf7d7, id("mission_main")));
	}
	else if (IsSpaceGame()) {
		CommManager.ShowCommEvent(CommManager.CreateSpaceCommEvent(0x0, planetID, id("civ_main_menu"), id("player_contact_npc")));
	}
	else {
		/*
		SporeDebugPrint("cCommManager is nullptr: %b", cCommManager::Get() == nullptr);
		auto empire = StarManager.GetEmpireForStar(starRecord);
		//auto event = CommManager.CreateSpaceCommEvent(0x0, planetID, id("civ_main_menu"), id("player_contact_npc"));
		auto event = CommManager.CreateCivCommEvent(0x0, 0x0, planetID, id("civ_main_menu"), id("player_contact_npc"));

		commUIlayout.LoadByID(id("CommScreen-3"));
		spaceUIlayout.LoadByID(0x1E453B88);


		CommManager.ShowCommEvent(event); */
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
