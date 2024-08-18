#include "stdafx.h"
#include "CRG_PosessTarget.h"

CRG_PosessTarget::CRG_PosessTarget()
{
}


CRG_PosessTarget::~CRG_PosessTarget()
{
}


void CRG_PosessTarget::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		if (avatar->GetAnimalTarget()) {
			GameNounManager.SetAvatar(avatar->GetAnimalTarget());
		}
		else {
			App::ConsolePrintF("This cheat requires a valid target!");
		}
	}
}

const char* CRG_PosessTarget::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Makes the targeted creature become the new avatar.";
	}
	else {
		return "PosessTarget: Makes the targeted creature become the new avatar.";
	}
}
