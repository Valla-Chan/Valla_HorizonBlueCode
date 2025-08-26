#pragma once

#include <Spore\BasicIncludes.h>
#include "CreatureSpeedBoost.h"
#include "TRG_TraitTables.h"
#include "TRG_ChieftainManager.h"

#define cTribeMemberManagerPtr intrusive_ptr<cTribeMemberManager>
#define TribeMemberManager (*cTribeMemberManager::Get())

using namespace Simulator;

class cTribeMemberManager
	: public Simulator::cStrategy
	, public TraitTable
{
public:
	//static const uint32_t TYPE = id("cTribeMemberManager");
	static const uint32_t TYPE = id("Valla_CreatureOverhaul::TRG_MemberManager");
	static const uint32_t NOUN_ID = TYPE;

	cTribeMemberManager();
	~cTribeMemberManager();

	int AddRef() override;
	int Release() override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;
	static cTribeMemberManager* Get();

	void Reset();
	//--------------------------------------------------------------------------------------------------
	// This file manages all tribe members, and saves the data with the world.

	// SUBMANAGERS
	//---------------------------------------

	TRG_ChieftainManager* trg_chiefmanager;

	//---------------------------------------

	const ResourceKey IDcolorsKey = ResourceKey(id("MemberColors"), TypeIDs::Names::prop, id("TribeTraits"));

	vector<ColorRGB> mIDcolors;

	//--------------------------------------------------------------------------------------------------
	// PERSONALITY TRAITS
	//--------------------------------------------------------------------------------------------------

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
	
	struct DesiredToolState {

		int mToolTypeID = -1;
		bool mbGrabbing = false;

		DesiredToolState(int id = -1) {
			mToolTypeID = id;
			mbGrabbing = false;
		}

		static Simulator::Attribute ATTRIBUTES[];

	};

	//------------------------------------------

	eastl::hash_map<uint32_t, MemberPersonality> mCreaturePersonalities;
	// when a creature gains the action to grab a tool, store that creature ID and tool ID here. handled mostly externally in a CitizenDoAction detour
	eastl::hash_map<uint32_t, DesiredToolState> mCreatureDesiredTool;
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

	// Tool Usage
	void StoreCreatureDesiredTool(cCreatureCitizenPtr creature, int tooltype);
	void SetCreatureDesiredToolGrabState(cCreatureCitizenPtr creature, bool grabbing);
	void RemoveCreatureDesiredTool(cCreatureCitizenPtr creature);
	// gets the creature's desired tool from the list. return -1 if not found
	int GetCreatureDesiredTool(cCreatureCitizenPtr creature, bool requiregrabbed = false);

	//-------------------------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];

protected:
	static cTribeMemberManager* sInstance;

private:
	intrusive_ptr<App::ScheduledTaskListener> mBabyFXSuppressTask;

};