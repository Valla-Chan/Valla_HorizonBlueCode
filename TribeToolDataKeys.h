#pragma once

using namespace Simulator;

const eastl::hash_set<uint32_t> defaultTribeTools = {
		id("attack1"),
		id("attack2"),
		id("attack3"),
		id("social1"),
		id("social2"),
		id("social3"),
		id("chieftain"),
		id("defaulttool"),
		id("firepit"),
		id("fish"),
		id("gather"),
		id("heal"),
};

const hash_map<int, uint32_t> defaultTribeToolEffectIDs = {
	{ kHandheldItemNone, 0x0 },
	{ kHandheldItemTrgHuntingTool1, id("trg_hunting_tool1") },
	{ kHandheldItemTrgHuntingTool2, id("trg_hunting_tool2") },
	{ kHandheldItemTrgHuntingTool3, id("trg_hunting_tool3") },
	{ kHandheldItemTrgFishingTool, id("trg_hunting_tool1") },
	{ kHandheldItemTrgFish, id("trg_fishing_tool1_fish") }, /// Either trg_eating_fish or trg_fishing_tool1_fish }, etc
	{ kHandheldItemTrgSeaweed, id("trg_fishing_tool1_seaweed") }, /// Either trg_eating_seaweed or trg_fishing_tool1_seaweed }, etc
	{ kHandheldItemTrgEatingFruit, id("trg_eating_fruit") },
	{ kHandheldItemTrgEatingSeaweedOrFish, id("trg_eating_fish") }, /// trg_eating_seaweed or trg_eating_fish
	{ kHandheldItemTrgEatingMeat, id("trg_eating_meat") },
	{ kHandheldItemTrgEatingSeaweed, id("trg_eating_seaweed") },
	{ kHandheldItemCityProtestSignHunger, id("city_protest_sign_hunger") },
	{ kHandheldItemTrgRecruitStaff, id("trg_recruit_staff") },
	{ kHandheldItemTrgSocialMaraca, id("trg_social_maraca") },
	{ kHandheldItemTrgSocialHorn, id("trg_social_horn") },
	{ kHandheldItemTrgSocialDidgeridoo, id("trg_social_didgeridoo") },
	{ kHandheldItemTrgGatheringTreeSmacker, id("trg_gathering_treesmacker") },
	{ kHandheldItemTrgChieftainStaff, id("trg_chieftain_staff") },
	{ kHandheldItemTrgHealingStaff, id("trg_healing_staff") },
	{ kHandheldItemTrgRepairMallet, id("trg_repair_mallet") },
	{ kHandheldItemTrgWaterBucket, id("trg_water_bucket") },
	{ kHandheldItemTrgWildHorn, id("trg_wildhorn") },
	{ kHandheldItemTrgFireBomb, id("trg_firebomb") },
	{ kHandheldItemTrgEatingEgg, id("trg_eating_egg") },
};

// Build progress models
// TODO: make these actually different between types. maybe even data driven, if needed to have ghosting FX...
const ResourceKey plot_social01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
const ResourceKey plot_social02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));

const ResourceKey plot_attack01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
const ResourceKey plot_attack02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));

const ResourceKey plot_heal01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
const ResourceKey plot_heal02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));

const ResourceKey plot_food01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
const ResourceKey plot_food02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));

const float progress_value = 0.67f; // starter health scale for hut construction. below this health percent, creatures throw buckets of water.