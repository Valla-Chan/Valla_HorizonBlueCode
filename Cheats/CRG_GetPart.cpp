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
	if (!Simulator::IsCreatureGame() || !GameNounManager.GetAvatar()) { return; }

	mParameter = line.GetArguments(1)[0];
	int level = 1;
	if (mParameter != "") {
		level = mpFormatParser->ParseInt(mParameter);
	}

	App::CreatureModeStrategies::UnlockPart action = { GameNounManager.GetAvatar(), 0, level };
	CreatureModeStrategy.ExecuteAction(action.ID, &action);
}


const char* CRG_GetPart::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Unlocks a new part. Supply an integer argument for the part unlock level.";
	}
	else {
		return "GivePart: Unlocks a new part for your creature. Supply an integer argument for the part unlock level.";
	}
}
