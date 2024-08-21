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
		App::ConsolePrintF("Searching for convertible items...");

		auto interactables = GetData<cInteractiveOrnament>();
		App::ConsolePrintF("interactable objects: %i", interactables.size());
		App::ConsolePrintF("searching for model: %x", mdl_carcass01);
		for (auto object : interactables) {
			App::ConsolePrintF("Found model: %x", object->GetModelKey().instanceID);
			if (object->GetModelKey().instanceID == mdl_carcass01) {
				App::ConsolePrintF("Found a carcass.");
			}
		}
		//Simulator::GetActivePlanetRecord()->mWaterScore = 1.0f;
		//Simulator::GetActivePlanetRecord()->mTemperatureScore = 1.0f;
		//Simulator::GetActivePlanetRecord()->mAtmosphereScore = 0.0f;
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
