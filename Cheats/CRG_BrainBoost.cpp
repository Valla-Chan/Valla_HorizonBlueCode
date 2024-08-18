#include "stdafx.h"
#include "CRG_BrainBoost.h"
#include <Spore/Simulator/cCreatureGameData.h>

CRG_BrainBoost::CRG_BrainBoost()
{
}


CRG_BrainBoost::~CRG_BrainBoost()
{
}


void CRG_BrainBoost::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsCreatureGame() && GameNounManager.GetAvatar()) {
		Simulator::cCreatureGameData::AddEvolutionPoints(Simulator::cCreatureGameData::GetEvoPointsToNextBrainLevel());
	}
}

const char* CRG_BrainBoost::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat advances your creature to the next brain level milestone.";
	}
	else {
		return "BrainBoost: This cheat advances your creature to the next brain level milestone.";
	}
}
