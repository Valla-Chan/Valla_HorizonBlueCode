#pragma once

#include <Spore\BasicIncludes.h>

// animations of the default captain hover / jet
const eastl::vector<uint32_t> hover_animations = {
	0x0772FF58, //"scenario_sentient_jumpjet"
	0x07E07EA3, //"scenario_creature_jumpjet"

	//0x077300F2, //"EP1_trader_hover_start"
	//0x077300FB, //"EP1_trader_hover_loop"
	//0x077300FF, //"EP1_trader_hover_impulse"
	//0x07730105, //"EP1_trader_hover_land"
};

// new animations for the jet hover
const eastl::vector<uint32_t> jet_animations = {
	0x783C6E1E, //"ep1_trader_jumpjet_nov"
	0x783C6E1E, //"ep1_trader_jumpjet_nov"

	//0x34E7EAED, //"EP1_trader_jumpjet_hoverburst"
	//0x0692E6A9, //"gen_jump_loop"
	//0x34E7EAED, //"EP1_trader_jumpjet_hoverburst"
	//0x0773009A, //"EP1_trader_jumpjet_land"
};

uint32_t JetHover_GetAnim(uint32_t animID) {
	for (size_t i = 0; i < hover_animations.size(); i++) {
		if (hover_animations[i] == animID) {
			return jet_animations[i];
		}
	}
	return animID;
}
