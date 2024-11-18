#pragma once

#include <Spore\BasicIncludes.h>

#define TRG_MemberManagerPtr intrusive_ptr<TRG_MemberManager>

///
/// In your dllmain Initialize method, add the system like this:
/// ModAPI::AddSimulatorStrategy(new TRG_MemberManager(), TRG_MemberManager::NOUN_ID);
///

using namespace Simulator;

class TRG_MemberManager
	: public Simulator::cStrategy
{
public:
	static const uint32_t TYPE = id("Valla_CreatureOverhaul::TRG_MemberManager");
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
	//--------------------------------------------------------------------------
	// This file manages all tribe members, and saves the data with the world.

	const ResourceKey IDcolorsKey = ResourceKey(id("MemberColors"), TypeIDs::Names::prop, id("TribeTraits"));

	vector<ColorRGB> mIDcolors;

	// Traits
	enum Traits : uint32_t
	{
		// Combat related
		Tough,
		Aggressive,
		Hardy,
		Frail,

		// Social Related
		Social,
		Flirty,
		Emotional,

		// Movement Related
		Athletic,

		// Resource Related
		Glutton,
		Drunkard,
		Cannibal,
		
		// Misc
		Stupid,
		Unsanitary,
		Nocturnal,
		Religious,
		Secular,
		Lucky,
		AnimalLover,

		// Do not use
		End,
	};

	const eastl::hash_map<uint32_t, ColorRGB> mTraitList = {
		{ Tough, ColorRGB(0xb47c30) },		// tan
		{ Aggressive, ColorRGB(0x9d0505) },	// scarlet
		{ Hardy, ColorRGB(0xe03a51) },		// pale maroon
		{ Frail, ColorRGB(0xd9bf44) },		// pale yellow

		{ Social, ColorRGB(0xd9bf44) },		// leaf green
		{ Flirty, ColorRGB(0xf27bb1) },		// light pink
		{ Emotional, ColorRGB(0x44296d) },	// dark purple

		{ Athletic, ColorRGB(0x54e1ea) },	// cyan

		{ Glutton, ColorRGB(0xec7e15) },	// medium orange
		{ Drunkard, ColorRGB(0x64822a) },	// dark green-yellow
		{ Cannibal, ColorRGB(0x0) },	// unset

		{ Stupid, ColorRGB(0x737373) },		// gray
		{ Unsanitary, ColorRGB(0xd1c38a) },	// slight yellow-white, use as tint instead of identity
		{ Nocturnal, ColorRGB(0x000000) },	// black
		{ Secular, ColorRGB(0xd5d5d5) },	// offwhite
		{ Lucky, ColorRGB(0xfbb53e) },		// dandelion
		{ AnimalLover, ColorRGB(0x0) },		// unset
	};

	//----------------------------------------------------------------------

	struct MemberPersonality {

		cCreatureCitizenPtr mpCreature;

		bool valid = false;
		vector<int> mTraits = {};
		ColorRGB mIDColor;

		MemberPersonality() {};

		MemberPersonality(cCreatureCitizenPtr creature, ColorRGB color) {
			valid = true;
			mpCreature = creature;
			mIDColor = color;
		}

		static Simulator::Attribute ATTRIBUTES[];

	};

	//----------------------------------------------------------------------

	eastl::hash_map<uint32_t, MemberPersonality> mCreaturePersonalities; // cCreatureCitizenPtr

	//vector<MemberPersonality> mCreaturePersonalities = {};
	vector<cCreatureCitizenPtr> mCurrentBabies;

	//-------------------------------------------------------------------


	void LoadIDColors();

	void StoreCurrentBabies();
	cCreatureCitizenPtr GetGrownBaby();

	ColorRGB GetRandColor() const;
	void AssignColor(cCreatureCitizenPtr creature);

	// Personality

	void AssignPersonality(cCreatureCitizenPtr creature);
	void ApplyPersonality(MemberPersonality& personality);
	void ApplyAllPersonalities();
	MemberPersonality GetPersonality(cCreatureCitizenPtr creature) const;

	//-------------------------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];

private:

};


