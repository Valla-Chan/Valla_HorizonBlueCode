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
