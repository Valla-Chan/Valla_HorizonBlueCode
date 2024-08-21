#include "stdafx.h"
#include "CRG_BrainDrain.h"
#include <Spore/Simulator/cCreatureGameData.h>

CRG_BrainDrain::CRG_BrainDrain()
{
}


CRG_BrainDrain::~CRG_BrainDrain()
{
}


void CRG_BrainDrain::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsCreatureGame() && GameNounManager.GetAvatar()) {
		if (Simulator::cCreatureGameData::GetEvolutionPoints() > 0) {
			float points = 100.0;
			if (Simulator::cCreatureGameData::GetEvolutionPoints() < 100) {
				points = Simulator::cCreatureGameData::GetEvolutionPoints();
			}
			Simulator::cCreatureGameData::AddEvolutionPoints(-1.0*points);
		}
		
		Simulator::cCreatureGameData::AddEvolutionPoints(Simulator::cCreatureGameData::GetEvoPointsToNextBrainLevel());
	}
}

const char* CRG_BrainDrain::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Drops your creature's evolution points by 100.";
	}
	else {
		return "BrainDrain: Drops your creature's evolution points by 100.";
	}
}