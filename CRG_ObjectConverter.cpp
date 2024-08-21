#include "stdafx.h"
#include "CRG_ObjectConverter.h"
#include "CRG_ObjectManager.h"

virtual_detour(AnimOverride_detour, Anim::AnimatedCreature, Anim::AnimatedCreature, void(uint32_t, int*)) {
	void detoured(uint32_t animID, int* pChoice) {

		if (IsCreatureGame()) {
			cCreatureAnimal* avatar = GameNounManager.GetAvatar();
			if (avatar && animID == 0x03DF6DFF) { //gen_dig_hands


				// TODO: this currently crashes due to a missing object
				//ObjectManager.StartWaitingForNoun();


				original_function(this, 0x04F65995, pChoice); //eat_meat_mouth_01
				return;
			}
		}
		original_function(this, animID, pChoice);

	}
};

void AddConverterDetours() {
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
}

/*
void CRG_ObjectConverter::AttachDetour() {
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
}*/
