#pragma once

#include <Spore\BasicIncludes.h>

#define CRG_EnergyHungerSyncPtr intrusive_ptr<CRG_EnergyHungerSync>

class CRG_EnergyHungerSync 
	: public Object
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("CRG_EnergyHungerSync");
	
	CRG_EnergyHungerSync();
	~CRG_EnergyHungerSync();

	void Update() override;

	bool has_synced_once = false;

	float energy_f = 0.0;
	float hunger_f = 100.0;
	float prev_energy_f = 0.0;
	float prev_hunger_f = 100.0;

	int caplvl_energyregen = 0;
	float energy_to_restore = 0.0f;

	float CRG_EnergyHungerSync::GetEnergyFloat() const;
	float CRG_EnergyHungerSync::GetHungerFloat() const;

	void CRG_EnergyHungerSync::SetEnergyFloat(float p_energy);
	void CRG_EnergyHungerSync::SetHungerFloat(float p_hunger);

	virtual int AddRef() override;
	virtual int Release() override;
	void* Cast(uint32_t type) const override;
};
