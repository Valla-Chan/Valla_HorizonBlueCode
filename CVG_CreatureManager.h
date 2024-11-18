#pragma once

#include <Spore\BasicIncludes.h>

#define CVG_CreatureManagerPtr intrusive_ptr<CVG_CreatureManager>


using namespace UTFWin;
using namespace Simulator;
using namespace TypeIDs;
using namespace App;
using namespace UI;
using namespace Sporepedia;
using namespace Sporepedia::OTDB;

class CVG_CreatureManager
	: public Simulator::cStrategy
	, public DefaultRefCounted
	//, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("Valla_CreatureOverhaul::CVG_CreatureManager");
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
	//------------------------------------------------

	// Saved vars
	//float mTimeOfDay;

	cCreatureCitizenPtr TraceHitCitizen() const;
	
	bool IsCreatureIndexUsed(int index) const;
	cSpeciesProfile* GetRandomTribeSpecies(cSpeciesProfile* species);
	int DetermineNeededCityType() const;
	void NewCityAppeared();

	//bool HandleMessage(uint32_t messageID, void* msg) override;

	//------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];

private:
	const float mPlayerSpeciesChance = 0.4f;
	vector<Vector3> mClosestCityCreaturePositions = {};

	vector<int> used_creature_indexes = {};
	bool found_all_tribe_creatures = false;
};