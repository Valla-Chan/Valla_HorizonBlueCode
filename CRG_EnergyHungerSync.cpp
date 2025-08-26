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

void CRG_EnergyHungerSync::Update()
{
	if (!(Simulator::IsCreatureGame() || Simulator::IsTribeGame())) { return; }

	if (Simulator::IsCreatureGame()) {
		cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
		if (!avatar) {
			has_synced_once = false;
		}
		SyncCreatureHungerEnergy(avatar);
	}
	else if (Simulator::IsTribeGame()) {
		auto playertribe = GameNounManager.GetPlayerTribe();
		if (playertribe) {
			for (auto member : playertribe->GetTribeMembers()) {
				SyncCreatureHungerEnergy(member);
			}
		}
		else {
			has_synced_once = false;
		}
		
	}
}

void CRG_EnergyHungerSync::SyncCreatureHungerEnergy(cCreatureBasePtr creature) {
	// Link Energy and Hunger
	// TODO: solve this breaking on respawn
	if (creature) {

		// Store prev floats, then update floats with new values from avatar
		prev_energy_f = energy_f;
		prev_hunger_f = hunger_f;
		energy_f = creature->mEnergy / creature->mMaxEnergy;
		hunger_f = creature->mHunger / 100.0f;

		// Always sync energy to hunger when loading in.
		if (!has_synced_once) {
			App::ConsolePrintF("Recalculating Energy / Hunger base values.");
			has_synced_once = true;
			prev_energy_f = energy_f;
			prev_hunger_f = hunger_f;
			SetEnergyFloat(hunger_f);
			energy_to_restore = 0.0f;
			// NOTE: this value needs to update after going into the editor!
			//caplvl_energyregen = CapabilityChecker::GetCapabilityLevel(avatar, 0x073CE5DD);
			return;
		}

		// Handle Manual Energy Recharge
		// This is currently not being used, but it can be reactivated for custom behaviors
		/*
		if (caplvl_energyregen > 0 && avatar->mEnergy < avatar->mMaxEnergy && energy_to_restore > 0.0f) {
			float energy_per_tick = 0.0;
			energy_per_tick = caplvl_energyregen * 0.15f * GameTimeManager.GetSpeed();
			// clamp the energy per tick to the max amount to restore.
			if (energy_per_tick > energy_to_restore) { energy_per_tick = energy_to_restore; }
			if (energy_per_tick > 0.00001) {
				energy_to_restore -= energy_per_tick;
				avatar->mEnergy += energy_per_tick;
			}
		}
		*/

		// If there is a mismatch in hunger/energy, correct values based on the differences
		// NOTE: the difference threshhold here can be turned up to block automatic recharging
		if (abs(energy_f - hunger_f) > 0.001f && (energy_f != prev_energy_f || hunger_f != prev_hunger_f)) {
			//App::ConsolePrintF("update hunger/energy from differences");
			float diffs = (energy_f - prev_energy_f) + (hunger_f - prev_hunger_f);

			/*
			// Manual Energy Recharge
			// If only the energy has dropped, allow that much to be restored.
			if (prev_energy_f - energy_f > 0.1) {
				energy_to_restore += abs(prev_energy_f - energy_f)*avatar->mMaxEnergy;
			}*/

			SetEnergyFloat(prev_hunger_f + diffs);
			SetHungerFloat(prev_hunger_f + diffs);

			// update these values for next time
			energy_f = creature->mEnergy / creature->mMaxEnergy;
			hunger_f = creature->mHunger / 100.0f;
		}

	}
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
