#pragma once

#include <Spore\BasicIncludes.h>
#include "CreatureSpeedBoost.h"

#define TRG_FireDanceManagerPtr intrusive_ptr<TRG_FireDanceManager>

using namespace Simulator;

class TRG_FireDanceManager
	: public Simulator::cStrategy
{
public:
	static const uint32_t TYPE = id("TRG_FireDanceManager");
	static const uint32_t NOUN_ID = TYPE;

	int AddRef() override;
	int Release() override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;

	//---------------------------------------------------

	const float BonusLifetime = 3.0f;

	enum DanceAnims {
		soc_fire_dance_turn_fast = 0x2c39830, // 2nd to last
		soc_celebrate_trg = 0x2c39521, // last
	};

	enum Bonus {
		None,
		Defense,
		Damage,
		Social,
		Speed,
	};

	Bonus mCurrentBonus = Speed;
	// TODO: we need to clear these after a timer from when the first creature was added.
	vector<cCreatureCitizenPtr> mpDancers = {};
	vector<cCreatureCitizenPtr> mpBuffedCreatures = {};
	

	//---------------------------------------------------
	// TODO: only allow the fire dance to be carried out once a day.

	bool IsDancer(cCreatureCitizenPtr creature) const;

	void AddDancer(cCreatureCitizenPtr creature);
	void ClearDancers();
	void HitLastDanceAnim(cCreatureCitizenPtr creature);

	// Effects
	void ShowFireBonusEffect();

	void ApplyBonusToAllDancers();
	void ResetAllBonuses();
	void ApplyBonusToDancer(cCreatureCitizenPtr creature);
	void ResetBonusForCreature(cCreatureCitizenPtr creature, TRG_FireDanceManager::Bonus bonus);

	//---------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];

private:
	intrusive_ptr<Simulator::ScheduledTaskListener> mRemoveDancersTask;
};