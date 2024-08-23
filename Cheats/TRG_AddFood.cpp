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
		//TODO: how to add food?
		cTribePtr tribe = Simulator::GetDataByCast<Simulator::cTribe>()[0];
		//tribe->mpTribePlanner->
	}
}

const char* TRG_AddFood::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Adds food to the active tribe in tribal stage.";
	}
	else {
		return "AddFood: Adds food to the active tribe in tribal stage.";
	}
}
