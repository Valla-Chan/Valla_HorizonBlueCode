#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"

using namespace Simulator;

bool debugMode = false;
class HBDebugFuncs
{

public:

	static void Initialize() {
	}

	static void Dispose() {
	}

	static void AttachDetours();
};

//---------------
// Detour Helpers

static bool Debug_AnimOverride_detour(Anim::AnimatedCreature* obj, uint32_t& animID, int* pChoice) {
	if (!debugMode) { return false; }
	if (GameNounManager.GetPlayerTribe()) {

		auto creature = Common::GetAnimCreatureCitizenOwner(obj);
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
		auto creature = Common::GetAnimCreatureOwner(obj);
		if (creature == hoveredAnimal) {
			//SporeDebugPrint("Hovered Creature Animation: %x", animID);
		}

	}
	return false;
}

static void Debug_EffectOverride_detour(uint32_t& instanceId) {
	if (!debugMode) { return; }
	// Print FX
	if (instanceId != 0x0) { SporeDebugPrint("Effect played: 0x%x", instanceId); }
}

/*
// Detour the herd spawning func
// TODO: NOT CURRENTLY USED
member_detour(HerdSpawn_detour, Simulator::cHerd, cHerd*(const Vector3&, cSpeciesProfile*, int, bool, int, bool)) {
	cHerd* detoured(const Vector3 & position, cSpeciesProfile* pSpeciesProfile, int herdSize, bool isOwnedByAvatar, int creaturePersonality, bool createNest)
	{
		//App::ConsolePrintF("Herd Spawned");
		cHerd* herd = original_function(this, position, pSpeciesProfile, herdSize, isOwnedByAvatar, creaturePersonality, createNest);
		return herd;

		// TODO: herd variety
	}
};*/

//-----------------
// ATTACH DETOURS
void HBDebugFuncs::AttachDetours() {
}
