#include "stdafx.h"
#include "TRG_AddFood.h"
#include <Spore\Simulator\cTribe.h>

TRG_AddFood::TRG_AddFood()
{
}


TRG_AddFood::~TRG_AddFood()
{
}


void TRG_AddFood::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsTribeGame())
	{
		cTribePtr tribe = GameNounManager.GetPlayerTribe();
		if (tribe) {
			float amount = 25.0f;
			auto tribe = GameNounManager.GetPlayerTribe();
			BundleManager.CreateBundles(amount, tribe, Simulator::kTribeBundleTypeFruit);
			tribe->UpdateFoodVisuals(amount);
		}
	}
}

const char* TRG_AddFood::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Adds 25 food to the active tribe in tribal stage.";
	}
	else {
		return "AddFood: Adds 25 food to the active tribe in tribal stage.";
	}
}
