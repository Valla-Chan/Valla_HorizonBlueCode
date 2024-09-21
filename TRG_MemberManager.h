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


	enum Traits {
		Stupid,
		LikesAnimals,
		Tough,
		Aggressive,
		Social,
		Flirty,
		Emotional,
		Unsanitary,
		Nocturnal,
		Hardy,
		Frail,
	};

	struct MemberPersonality {

		cCreatureCitizenPtr mpCreature;

		// auto set from the creature pointer
		string16 mName;
		//int mSkill // firestarter, axes, etc
		//float mSkillProficiency

		vector<int> mTraits = {};
		ColorRGB mFavColor;

		/*
		bool Write(Simulator::ISerializerStream* stream)
		{
			return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
		}
		bool Read(Simulator::ISerializerStream* stream)
		{
			return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
		}

		MemberPersonality() {

		}

		Simulator::Attribute ATTRIBUTES[] = {
			// Add more attributes here
			SimAttribute(MemberPersonality, mpCreature, 0),
			SimAttribute(MemberPersonality, mName, 1),
			SimAttribute(MemberPersonality, mTraits, 2),
			SimAttribute(MemberPersonality, mFavColor, 2),
			// This one must always be at the end
			Simulator::Attribute()
		};
		*/
	};

	vector<MemberPersonality> mCreaturePersonalities = {};


	static Simulator::Attribute ATTRIBUTES[];

private:

};