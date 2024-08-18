#include "stdafx.h"
#include "CRG_EnergyHungerSync.h"
#include "CapabilityChecker.h"

CRG_EnergyHungerSync::CRG_EnergyHungerSync()
{
	App::AddUpdateFunction(this);
	//App::ConsolePrintF("Initializing Hunger / Energy synchronization.");
}


CRG_EnergyHungerSync::~CRG_EnergyHungerSync()
{
}

// For internal use, do not modify.
void CRG_EnergyHungerSync::Update()
{
	if (!Simulator::IsCreatureGame()) { return; }
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();

	//Link Energy and Hunger
	if (avatar) {

		// store prev floats, then update floats with new values
		prev_energy_f = energy_f;
		prev_hunger_f = hunger_f;
		energy_f = avatar->mEnergy / avatar->mMaxEnergy;
		hunger_f = avatar->mHunger / 100.0f;

		// always sync energy to hunger when loading in.
		if (!has_synced_once) {
			has_synced_once = true;
			prev_energy_f = energy_f;
			prev_hunger_f = hunger_f;
			SetEnergyFloat(hunger_f);
			return;
		}

		// there is a mismatch in hunger/energy, correct values based on the differences
		if ((round(energy_f*100) != (round(hunger_f * 100))) && (energy_f != prev_energy_f || hunger_f != prev_hunger_f)) {
			//App::ConsolePrintF("update hunger/energy from differences");
			float diffs = (energy_f - prev_energy_f) + (hunger_f - prev_hunger_f);
			SetEnergyFloat(prev_hunger_f + diffs);
			SetHungerFloat(prev_hunger_f + diffs);

			// update these values for next time
			energy_f = avatar->mEnergy / avatar->mMaxEnergy;
			hunger_f = avatar->mHunger / 100.0f;
		}

	}
	else { has_synced_once = false; }
}


// SETTERS

void CRG_EnergyHungerSync::SetEnergyFloat(float p_energy) {
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->mEnergy = p_energy * avatar->mMaxEnergy;
	}
}

void CRG_EnergyHungerSync::SetHungerFloat(float p_hunger) {
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->mHunger = p_hunger * 100.0f;
	}
}

// GETTERS

float CRG_EnergyHungerSync::GetEnergyFloat() const {
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		return avatar->mEnergy / avatar->mMaxEnergy;
	}
	else { return 0.0; }
}

float CRG_EnergyHungerSync::GetHungerFloat() const {
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		return avatar->mHunger / 100.0f;
	}
	else { return 100.0; }
}


// For internal use, do not modify.
int CRG_EnergyHungerSync::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CRG_EnergyHungerSync::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* CRG_EnergyHungerSync::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(CRG_EnergyHungerSync);
	return nullptr;
}
