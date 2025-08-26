#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"
// SPG Ingame Behaviors
// SPG Tool Strats
#include "ToolFrenzyBomb.h"

using namespace Simulator;

#define HBSpacePtr intrusive_ptr<HBSpace>

// SPG

class HBSpace
{

public:

	static void Initialize() {
		ToolManager.AddStrategy(new ToolFrenzyBomb(), id("ToolFrenzyBomb"));
	}

	static void Dispose() {
	}

	static void AttachDetours();


};

//----------
// Detours


//-----------------
// ATTACH DETOURS
void HBSpace::AttachDetours() {

}
