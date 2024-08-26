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
		float points_to_next = Simulator::cCreatureGameData::GetEvoPointsToNextBrainLevel();
		if (points_to_next > 0) {
			Simulator::cCreatureGameData::AddEvolutionPoints(points_to_next);
		}
		else {
			GameNounManager.GetAvatar()->SetCurrentBrainLevel(6);
		}

	}
}

const char* CRG_BrainBoost::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Advances your creature to the next brain level milestone.";
	}
	else {
		return "BrainBoost: Advances your creature to the next brain level milestone.";
	}
}
