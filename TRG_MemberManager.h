#pragma once

#include <Spore\BasicIncludes.h>
#include "CreatureSpeedBoost.h"
#include "TRG_TraitTables.h"

#define TRG_MemberManagerPtr intrusive_ptr<TRG_MemberManager>

using namespace Simulator;

class TRG_MemberManager
	: public Simulator::cStrategy
	, public TraitTable
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

	//----------------------------------------------------------------------

	struct MemberPersonality {

		cCreatureCitizenPtr mpCreature;

		bool valid = false;
		vector<int> mTraits = {}; // use the integer to get a trait from the trait table.
		ColorRGB mIDColor;

		MemberPersonality() {};

		MemberPersonality(cCreatureCitizenPtr creature, ColorRGB color) {
			valid = true;
			mpCreature = creature;
			mIDColor = color;
		}
		//TraitData GetTraitIndexData(int index) const {
		//	return GetTraitData(mTraits[index]);
		//}

		static Simulator::Attribute ATTRIBUTES[];

	};

	//----------------------------------------------------------------------

	CreatureSpeedBoost* mCreatureSpeedBoost = nullptr;
	eastl::hash_map<uint32_t, MemberPersonality> mCreaturePersonalities; // cCreatureCitizenPtr
	vector<cCreatureCitizenPtr> mCurrentBabies;

	bool mbSuppressBabyGrowFX = false;

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
	void ApplyAllPersonalities2();
	void ResetSuppressBabyFX();
	MemberPersonality GetPersonality(cCreatureCitizenPtr creature) const;

	//-------------------------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];

private:
	intrusive_ptr<App::ScheduledTaskListener> mBabyFXSuppressTask;
};


