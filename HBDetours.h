#pragma once
#include "stdafx.h"
#include "Common.h"

// HB Common Detours


// Detour the playanimation ID-picking func
member_detour(AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

		// DEBUG:
//#ifdef DEBUG
		if (GameNounManager.GetPlayerTribe()) {

			auto creature = Common::GetAnimCreatureCitizenOwner(this);
			// only check anims on the chieftain!
			//if (GameNounManager.GetPlayerTribe()->GetLeaderCitizen() == creature) {

				// only check anims on the player tribe members
			if (creature && creature->mpOwnerTribe == GameNounManager.GetPlayerTribe()) {

				//SporeDebugPrint("Animation: %x", animID);
			}
			//}

		}
		auto hovered = GameViewManager.GetHoveredObject();
		auto hoveredAnimal = object_cast<cCreatureAnimal>(hovered);
		if (hoveredAnimal) {
			auto creature = Common::GetAnimCreatureOwner(this);
			if (creature == hoveredAnimal) {
				//SporeDebugPrint("Hovered Creature Animation: %x", animID);
			}

		}

		//#endif // DEBUG	

				//SporeDebugPrint("Animation: %x", animID);
		return original_function(this, animID, pChoice);

	}
};

// Spui loading/spawning detour
member_detour(ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey & name, bool arg_4, uint32_t arg_8) {
		

		return original_function(this, name, arg_4, arg_8);
	}
};



void AttachDetours()
{


}