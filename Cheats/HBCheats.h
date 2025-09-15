#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"

// Cheats
#include "HBdebug.h"
#include "BuildCRG.h"
#include "Kill.h"
#include "Hurtme.h"
#include "CRG_Starve.h"
#include "CRG_BrainBoost.h"
#include "CRG_BrainDrain.h"
#include "CRG_SwapCreature.h"
#include "CRG_PosessTarget.h"
#include "CRG_ResurrectTarget.h"
#include "CRG_Recharge.h"
#include "CRG_SpawnPlant.h"
#include "CRG_GrowUp.h"
#include "PrintCursor.h"
#include "SetCursorCheat.h"
#include "TRG_GetTribeInfo.h"
#include "SetChieftainColor.h"
#include "SetGlideSpeed.h"
#include "CRG_GetPart.h"
#include "CRG_GetSick.h"
#include "CRG_GiveAllParts.h"
#include "SetTimeScale.h"
#include "SetTime.h"
#include "TRG_GetClosestHerd.h"
#include "TRG_SelectMembers.h"
#include "TRG_AddToTribe.h"
#include "TRG_AddFood.h"
#include "TRG_UseTribalTool.h"
#include "SetVehicleTool.h"
#include "MarkInteractives.h"
#include "SendMessage.h"
#include "TRG_AddTribeSlot.h"
#include "GetTribeColors.h"
#include "Teleport.h"
#include "Rename.h"
#include "SetTechLevel.h"
#include "RepairAll.h"
#include "SetEmpireColor.h"
#include "HurtTarget.h"
#include "CVG_SetCityType.h"
#include "devPlayComm.h"
//#include "ListenAllMessages.h"#pragma once


class HBCheats
{

public:

	static void Initialize() {
		CheatManager.AddCheat("Build", new(BuildCRG));
		CheatManager.AddCheat("Kill", new(Kill));
		CheatManager.AddCheat("HurtMe", new(HurtMe));
		CheatManager.AddCheat("Starve", new(CRG_Starve));
		CheatManager.AddCheat("BrainBoost", new(CRG_BrainBoost));
		CheatManager.AddCheat("BrainDrain", new(CRG_BrainDrain));
		//CheatManager.AddCheat("SwapCreature", new(CRG_SwapCreature));
		CheatManager.AddCheat("PosessTarget", new(CRG_PosessTarget));
		CheatManager.AddCheat("Resurrect", new(CRG_ResurrectTarget));
		CheatManager.AddCheat("Recharge", new(CRG_Recharge));
		CheatManager.AddCheat("SpawnPlant", new(CRG_SpawnPlant));
		CheatManager.AddCheat("GrowUp", new(CRG_GrowUp));
		CheatManager.AddCheat("PrintCursor", new(PrintCursor));
		//CheatManager.AddCheat("SetCursor", new(SetCursorCheat));
		CheatManager.AddCheat("TribeInfo", new(TRG_GetTribeInfo));
		CheatManager.AddCheat("SetChieftainColor", new(SetChieftainColor));
		//CheatManager.AddCheat("SetGlideSpeed", new(SetGlideSpeed));
		CheatManager.AddCheat("GetSick", new(CRG_GetSick));
		CheatManager.AddCheat("GivePart", new(CRG_GetPart));
		CheatManager.AddCheat("GiveAllParts", new(CRG_GiveAllParts));
		CheatManager.AddCheat("TimeScale", new(SetTimeScale));
		CheatManager.AddCheat("TimeSet", new(SetTime));
		CheatManager.AddCheat("GetHerd", new(TRG_GetClosestHerd));
		CheatManager.AddCheat("SelectUnits", new(TRG_SelectMembers));
		CheatManager.AddCheat("AddToTribe", new(TRG_AddToTribe));
		CheatManager.AddCheat("AddFood", new(TRG_AddFood));
		CheatManager.AddCheat("UseTribeTool", new(TRG_UseTribalTool));
		CheatManager.AddCheat("SetVehicleTool", new(SetVehicleTool));
		CheatManager.AddCheat("MarkInteractives", new(MarkInteractives));
		CheatManager.AddCheat("SendMessage", new(SendMessageCheat));
		CheatManager.AddCheat("AddTribeSlot", new(TRG_AddTribeSlot));
		CheatManager.AddCheat("GetTribeColors", new(GetTribeColors));
		CheatManager.AddCheat("Teleport", new(Teleport));
		CheatManager.AddCheat("Rename", new(Rename));
		CheatManager.AddCheat("SetTechLevel", new(SetTechLevel));
		CheatManager.AddCheat("RepairAll", new(RepairAll));
		CheatManager.AddCheat("SetEmpireColor", new(SetEmpireColor));
		CheatManager.AddCheat("HurtTarget", new(HurtTarget));
		CheatManager.AddCheat("SetCityType", new(CVG_SetCityType));
		CheatManager.AddCheat("devPlayComm", new(devPlayComm));
		//CheatManager.AddCheat("ListenAllMessages", new(ListenAllMessages));

#ifdef _DEBUG
		CheatManager.AddCheat("HBdebug", new(HBdebug));
# endif
	}

	static void Dispose() {
	}

	static void AttachDetours();


};

//----------
// Detours


//-----------------
// ATTACH DETOURS
void HBCheats::AttachDetours() {

}
