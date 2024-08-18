#include "stdafx.h"
#include "CRG_SpawnPlant.h"
#include <Spore\Simulator\SimulatorEnums.h>

CRG_SpawnPlant::CRG_SpawnPlant()
{
}


CRG_SpawnPlant::~CRG_SpawnPlant()
{
}

// NOTE: this fundatmentally doesnt work atm, so i am using this to test random funcs.
void CRG_SpawnPlant::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();

	if (avatar) {
		/*
		cGamePlantPtr plant = object_cast<Simulator::cGamePlant>(GameNounManager.CreateInstance(Simulator::kGamePlant));
		plant->SetPosition(avatar->GetPosition());
		plant->SetModelKey(avatar->GetModelKey());
		GameNounManager.UpdateModels();
		*/

		//cCreatureAnimalPtr animal = Simulator::cCreatureAnimal::Create(avatar->GetPosition(), avatar->mpSpeciesProfile,avatar->mAge);
		
		/*
		cCreatureAnimalPtr creature = object_cast<Simulator::cCreatureAnimal>(GameNounManager.CreateInstance(Simulator::kCreatureAnimal));
		creature->SetPosition(avatar->GetPosition());
		creature->mSpeciesKey = avatar->mSpeciesKey;
		creature->SetModelKey(avatar->GetModelKey());
		GameNounManager.UpdateModels();
		*/
		Simulator::GetActivePlanetRecord()->mWaterScore = 1.0;
	}
}

const char* CRG_SpawnPlant::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat spawns a plant in front of the avatar.";
	}
	else {
		return "SpawnPlant: This cheat spawns a plant in front of the avatar.";
	}
}
