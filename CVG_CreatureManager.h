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
	, public IWinProc
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("Valla_CreatureOverhaul::CVG_CreatureManager");
	static const uint32_t NOUN_ID = TYPE;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;
	//------------------------------------------------

	bool mbMouseClicked = false;

	// Saved vars
	//float mTimeOfDay;

	const eastl::vector<uint32_t> civReactionAnims = {
		0x042A1CAF, // soc_sing_lvl01
		0x042A1CB2, // soc_sing_lvl04
		0x042DD1B8, // csa_emo_happy
		0x042DD1B8, // csa_emo_swoon
		0x042DD1B3, // csa_emo_scared
		0x042DD1AE, // csa_emo_laugh
		0x0431DF93, // csa_react_envir_01
		0x0431DF9A, // csa_react_envir_02
		0x0431DF9F, // csa_react_envir_03
		0x0431DFA6, // csa_react_envir_04
		0x04335859, // csa_mom_celebr
		0x044601D0, // csa_ed_addear1
		0x044601DA, // csa_ed_addear2
		0x044604F4, // csa_ed_addnose1
		0x04460503, // csa_ed_addnose2
		0x0446085D, // csa_ed_remov_antic
		0x044611DD, // csa_ed_remov_huh
		0x02481E14, // com_stun
		0x92314082, // gen_startled
	};

	bool IsPlayerCityInRange() const;
	bool IsPlayerCityHovered() const;
	cCreatureCitizenPtr GetHoveredCitizen(bool playerowned = true) const;
	cCreatureCitizenPtr TraceHitCitizen() const;
	
	bool IsCreatureIndexUsed(int index) const;
	cSpeciesProfile* GetRandomTribeSpecies(cSpeciesProfile* species);
	int DetermineNeededCityType() const;
	void NewCityAppeared();

	uint32_t GetReactionAnimation() const;
	void CitizenPlayReaction(cCreatureCitizenPtr citizen);


	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;

	//------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];

private:
	const float mPlayerSpeciesChance = 0.4f;
	vector<Vector3> mClosestCityCreaturePositions = {};

	vector<int> used_creature_indexes = {};
	bool found_all_tribe_creatures = false;
};