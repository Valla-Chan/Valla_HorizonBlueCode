#include "stdafx.h"
#include "CRG_ObjectConverter.h"
#include <Spore\Simulator\cInteractiveOrnament.h>
#include "CRG_ObjectManager.h"



virtual_detour(AnimOverride_detour, Anim::AnimatedCreature, Anim::AnimatedCreature, void(uint32_t, int*)) {
	void detoured(uint32_t animID, int* pChoice) {

		if (IsCreatureGame()) {
			cCreatureAnimal* avatar = GameNounManager.GetAvatar();
			if (avatar && animID == 0x03DF6DFF) { //gen_dig_hands


				// this currently crashes:
				//ObjectManager.waiting_for_noun = true;
				

				// loop through all ingame interactables
				//auto interactables = GetData<cInteractiveOrnament>();
				/*
				for (auto object : interactables) {
					App::ConsolePrintF("Selected: %x", object->IsSelected());

					//NOTE: this will NEVER return true.
					// find a new way to do this!!
					if (object->IsSelected()) {
						if (object->GetModelKey().instanceID == mdl_carcass01) {
							App::ConsolePrintF("Found a carcass.");
							object->SetScale(4);
						}
					}
				}
				*/

				original_function(this, 0x04F65995, pChoice); //eat_meat_mouth_01
				return;
			}
		}
		original_function(this, animID, pChoice);

	}
};

void AddConverterDetour() {
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
}

/*
void CRG_ObjectConverter::AttachDetour() {
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
}*/
