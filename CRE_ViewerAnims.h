#pragma once

#include <Spore\BasicIncludes.h>

// animations i do NOT want to play
const eastl::vector<uint32_t> replaced_animations = {
	0x042DD0EE, //"csa_actn_epicroar"
	0x04330328, //"csa_actn_battleroar"
};

// animations to still allow, but shuffle in the new ones as well
const eastl::vector<uint32_t> default_animations = {
	0x05DF99C1, //"spd_idle_cr_shake"
	0x068D8993, //"idle_gen_lookup"
	0x0519CBC2, //"idle_gen_lookrtlt"
	0x051870F2, //"idle_gen_lookrtlt_02"
	0x051870FA, //"idle_gen_lookltrt"
	0x05187102, //"idle_gen_lookltrt_02"
	0x0519E1DE, //"idle_gen_turn_v1"
	0x0519E1ED, //"idle_gen_turn_v2"
	0x0519E1FA, //"idle_gen_turn_v3"
	0x0519E1AC, //"idle_gen_sit_v1"
	0x0519E1BE, //"idle_gen_sit_v2"
};

// new animations
const eastl::vector<uint32_t> desired_animations = {
	0x731EA496, //"idle_herbi_scratch"
	0x731EA498, //"idle_herbi_snapflies"
	0x731EA499, //"idle_herbi_sniffair"
	0x731EA49A, //"idle_herbi_snort"
};


int last_idx = 0;

// if the anim should be replaced, return true 2/3 times.
bool ShouldReplaceAnim(uint32_t animID) {
	// high chance of replace
	for (auto item : replaced_animations) {
		App::ConsolePrintF("%x %x", item, animID);
		if (item == animID) {
			return bool(rand(3));
		}
	}
	// lower chance of replace
	for (auto item : default_animations) {
		App::ConsolePrintF("%x %x", item, animID);
		if (item == animID) {
			return randf() > 0.7;
		}
	}
	return false;
}


// Get a random replacer anim.
uint32_t GetRandViewerAnim()
{
	int idx = rand(desired_animations.size());
	// give 5 tries to shuffle it
	int tries = 5;
	while (last_idx == idx && tries > 0) {
		idx = rand(desired_animations.size());
		tries -= 1;
	}
	last_idx = tries;
	return desired_animations[idx];
}