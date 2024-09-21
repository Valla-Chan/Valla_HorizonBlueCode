#include "stdafx.h"
#include "TRG_SelectMembers.h"

TRG_SelectMembers::TRG_SelectMembers()
{
	MessageManager.AddListener(this, 0x06204441);
	MessageManager.AddListener(this, 0x064d50e0);
}


TRG_SelectMembers::~TRG_SelectMembers()
{
}

using namespace Simulator;
void TRG_SelectMembers::ParseLine(const ArgScript::Line& line)
{
	if (IsTribeGame()) {
		for (auto creature : GameNounManager.GetPlayerTribe()->GetTribeMembers()) {
			creature->SetIsSelected(true);
			 //comandselectallunits
		}
	}
	else if (IsCivGame()) {
		for (auto vehicle : GameNounManager.GetPlayerCivilization()->mVehicles) {
			vehicle->SetIsSelected(true);
		}
	}
}

bool TRG_SelectMembers::HandleMessage(uint32_t messageID, void* msg) {
	if (messageID == 0x06204441) {
		SporeDebugPrint("All units selected.");
	}
	else if (messageID == 0x064d50e0) {
		SporeDebugPrint("Single unit selected.");
	}
	return false;
}

const char* TRG_SelectMembers::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Selects all tribe members or vehicles in your control.";
	}
	else {
		return "SelectUnits: Selects all tribe members or vehicles in your control.";
	}
}
