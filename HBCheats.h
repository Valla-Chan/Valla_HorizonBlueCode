#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"
// Cheats
#include "HBdebug.h"
#include "Cheats/BuildCRG.h"
#include "Cheats/Kill.h"
#include "Cheats/Hurtme.h"
#include "Cheats/CRG_Starve.h"
#include "Cheats/CRG_BrainBoost.h"
#include "Cheats/CRG_BrainDrain.h"
#include "Cheats/CRG_SwapCreature.h"
#include "Cheats/CRG_PosessTarget.h"
#include "Cheats/CRG_ResurrectTarget.h"
#include "Cheats/CRG_Recharge.h"
#include "Cheats/CRG_SpawnPlant.h"
#include "Cheats/CRG_GrowUp.h"
#include "Cheats/PrintCursor.h"
#include "Cheats/SetCursorCheat.h"
#include "Cheats/TRG_GetTribeInfo.h"
#include "Cheats/SetChieftainColor.h"
#include "Cheats/SetGlideSpeed.h"
#include "Cheats/CRG_GetPart.h"
#include "Cheats/CRG_GetSick.h"
#include "Cheats/CRG_GiveAllParts.h"
#include "Cheats/SetTimeScale.h"
#include "Cheats/SetTime.h"
#include "Cheats/TRG_GetClosestHerd.h"
#include "Cheats/TRG_SelectMembers.h"
#include "Cheats/TRG_AddToTribe.h"
#include "Cheats/TRG_AddFood.h"
#include "TRG_UseTribalTool.h"
#include "Cheats/SetVehicleTool.h"
#include "Cheats/MarkInteractives.h"
#include "Cheats/SendMessage.h"
#include "Cheats/TRG_AddTribeSlot.h"
#include "Cheats/GetTribeColors.h"
#include "Cheats/Teleport.h"
#include "Cheats/Rename.h"
#include "Cheats/SetTechLevel.h"
#include "Cheats/RepairAll.h"
#include "Cheats/SetEmpireColor.h"
#include "Cheats/HurtTarget.h"
//#include "ListenAllMessages.h"

using namespace Simulator;

#define HBSpacePtr intrusive_ptr<HBSpace>

// SPG

class HBSpace
{

public:

	static void Initialize() {
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
