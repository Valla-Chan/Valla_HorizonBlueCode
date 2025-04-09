#include "stdafx.h"
#include "CRG_SpawnPlant.h"
#include <Spore\Simulator.h>
#include <Spore\Simulator\SimulatorEnums.h>
#include <Spore\Simulator\cInteractiveOrnament.h>

CRG_SpawnPlant::CRG_SpawnPlant()
{
}


CRG_SpawnPlant::~CRG_SpawnPlant()
{
}

// NOTE: this fundamentally doesnt work atm, so i am using this to test random funcs.
void CRG_SpawnPlant::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();

	if (avatar) {
		cVehiclePtr vehicle = simulator_new<Simulator::cVehicle>();
		vehicle->Load(kVehicleLand, kVehicleColony, avatar->GetModelKey());
		//vehicle->Load(kVehicleAir, kVehicleColony, avatar->GetModelKey());
		vehicle->Teleport(avatar->GetPosition(), avatar->GetOrientation());

		vehicle->mpChaseTarget = avatar;
		vehicle->SetTarget(avatar.get());
	}
	else {
		auto tribe = GameNounManager.GetPlayerTribe();
		if (tribe) {
			auto chieftain = tribe->GetLeaderCitizen();
			if (chieftain) {
				cVehiclePtr vehicle = simulator_new<Simulator::cVehicle>();
				vehicle->Load(kVehicleLand, kVehicleColony, chieftain->GetModelKey());
				vehicle->Teleport(chieftain->GetPosition(), chieftain->GetOrientation());

				vehicle->mpChaseTarget = chieftain;
				vehicle->SetTarget(chieftain);
			}
		}
	}
}

const char* CRG_SpawnPlant::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Spawns a plant in front of the avatar.";
	}
	else {
		return "SpawnPlant: Spawns a plant in front of the avatar.";
	}
}
