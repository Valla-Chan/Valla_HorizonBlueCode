#include "stdafx.h"
#include "CRG_GetPart.h"

CRG_GetPart::CRG_GetPart()
{
}


CRG_GetPart::~CRG_GetPart()
{
}


void CRG_GetPart::ParseLine(const ArgScript::Line& line)
{
	if (!Simulator::IsCreatureGame()) { return; }

	App::CreatureModeStrategies::UnlockPart action = { GameNounManager.GetAvatar(), 0, 1 }; // value 1 crashes, value 2 does not
	CreatureModeStrategy.ExecuteAction(action.ID, &action);
}

const char* CRG_GetPart::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Unlocks a new part.";
	}
	else {
		return "GivePart: Unlocks a new part for your creature.";
	}
}
