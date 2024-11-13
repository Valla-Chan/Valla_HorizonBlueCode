#include "stdafx.h"
#include "RepairAll.h"


RepairAll::RepairAll()
{
}


RepairAll::~RepairAll()
{
}


void RepairAll::ParseLine(const ArgScript::Line& line)
{
	using namespace Simulator;

	if (IsTribeGame()) {
		auto tribe = GameNounManager.GetPlayerTribe();
		// Heal hut and tools to max health
		if (tribe) {
			tribe->GetHut()->SetHealthPoints(tribe->mpHut->mMaxHealthPoints);
			for (auto tool : tribe->GetTools()) {
				tool->SetHealthPoints(tool->mMaxHealthPoints);
			}
		}
	}
	else if (IsCivGame() || IsSpaceGame()) {
		for (auto city : GetData<cCity>())
		{
			// Heal city hall, buildings, and turret (player owned only)
			if (city->IsPlayerOwned()) {
				city->mpCityHall->SetHealthPoints(city->mpCityHall->mMaxHealthPoints);
				for (cBuildingPtr building : city->mBuildings) {
					building->SetHealthPoints(building->mMaxHealthPoints);
				}
				for (size_t i = 0; i < city->mTurrets.size(); i++ ) {
					city->mTurrets[i]->SetHealthPoints(city->mTurrets[i]->mMaxHealthPoints);
				}
			}
			
		}
		for (auto vehicle : GetDataByCast<cVehicle>())
		{
			// Heal all player owned vehicles
			if (vehicle->IsPlayerOwned()) {
				vehicle->SetHealthPoints(vehicle->mMaxHealthPoints);
			}

		}
	}
}

const char* RepairAll::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Repairs all player owned buildings, turrets, and vehicles to full health.";
	}
	else {
		return "RepairAll: Repairs all player owned buildings, turrets, and vehicles to full health.";
	}
}
