#include "stdafx.h"
#include "CRG_GetSick.h"

CRG_GetSick::CRG_GetSick()
{
}


CRG_GetSick::~CRG_GetSick()
{
}


void CRG_GetSick::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->mbIsDiseased = true;
	}
}

const char* CRG_GetSick::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Makes the avatar creature get sick.";
	}
	else {
		return "GetSick: Makes the avatar creature get sick.";
	}
}
