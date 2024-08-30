#include "stdafx.h"
#include "CRG_GiveAllParts.h"

CRG_GiveAllParts::CRG_GiveAllParts()
{
}


CRG_GiveAllParts::~CRG_GiveAllParts()
{
}


void CRG_GiveAllParts::ParseLine(const ArgScript::Line& line)
{
	if (!Simulator::IsCreatureGame() || !GameNounManager.GetAvatar()) { return; }

	for (int i = 0; i < 999; i++) {
		App::CreatureModeStrategies::UnlockPart action = { GameNounManager.GetAvatar(), 0, 4 };
		CreatureModeStrategy.ExecuteAction(action.ID, &action);
	}
	
}

const char* CRG_GiveAllParts::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Unlocks all available creature parts.";
	}
	else {
		return "GiveAllParts: Unlocks all available creature parts.";
	}
}
