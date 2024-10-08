#include "stdafx.h"
#include "TRG_AddTribeSlot.h"

TRG_AddTribeSlot::TRG_AddTribeSlot()
{
}


TRG_AddTribeSlot::~TRG_AddTribeSlot()
{
}


void TRG_AddTribeSlot::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsTribeGame()) {
		cTribePtr tribe = GameNounManager.GetPlayerTribe();
		auto pos = GameViewManager.GetWorldMousePosition(0, nullptr);

		cLayoutSlot newSlot;
		newSlot.field_8 = 0;
		newSlot.mIsOccupied = false;
		newSlot.mPosition = pos;

		tribe->mTribeLayout.mSlots.push_back(newSlot);
		tribe->mTribeLayout.SnapSlotsToPlanet();
	}
}

const char* TRG_AddTribeSlot::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Adds a tribe slot to the player tribe at the mouse position.";
	}
	else {
		return "AddTribeSlot: Adds a tribe slot to the player tribe at the mouse position.";
	}
}
