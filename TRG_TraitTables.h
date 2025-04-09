#include <Spore\BasicIncludes.h>
#pragma once

#define AnimMap eastl::hash_map<uint32_t, AnimationReplacer>
#define TraitDataMap eastl::hash_map<int, TraitData>

class TraitTable {
public:


	struct AnimationReplacer {

		uint32_t mAnimID = 0x0;
		float mPlayChance = 1.0f;

		AnimationReplacer(uint32_t id, float chance = 1.0f) {
			mAnimID = id;
			mPlayChance = chance;
		}
		AnimationReplacer(uint32_t id, int chance = 1) {
			mAnimID = id;
			mPlayChance = float(chance);
		}
	};

	// NOTE: the trait data will most likely be generated in this file when the game starts,
	// So there is no need to save this struct as a simulator object.
	struct TraitData {

		uint32_t mTraitID = 0x0;
		LocalizedString mTraitName;
		AnimMap mAnimMap;

		TraitData() {}
		TraitData(uint32_t id) {
			mTraitID = id;
			// TODO: automatically pull the trait name from the trait definition resource
			// via mTraitID as the instance, and a static groupID.
		}

		static Simulator::Attribute ATTRIBUTES[];
	};


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

	//----------------------------------------------------------------
	// Map the traits to their trait data.

	const TraitDataMap mTraitData = {

		{ Emotional, TraitData(0x0) }, // placeholder

	};

	
	TraitData GetTraitData(int trait) {
		auto E = mTraitData.find(trait);
		if (E != mTraitData.end()) {
			return E.get_node()->mValue.second;
		}
		return TraitData();
	}

	//----------------------------------------------------------------
	// Animations to replace for tribal creatures with these traits.


	AnimMap mTraitEmotional = {

			{ 0x0, AnimationReplacer(0xb47c30, 1.0f) }, // placeholder

	};
};