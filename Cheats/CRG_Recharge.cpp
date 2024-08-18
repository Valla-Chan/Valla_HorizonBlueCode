#include "stdafx.h"
#include "CRG_Recharge.h"

CRG_Recharge::CRG_Recharge()
{
}


CRG_Recharge::~CRG_Recharge()
{
}


void CRG_Recharge::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->mEnergy = avatar->mMaxEnergy;
		avatar->mRechargingAbilityBits.reset();
	}
}

const char* CRG_Recharge::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Recharges all energy and resets all ability cooldowns.";
	}
	else {
		return "Recharge: Recharges all energy and resets all ability cooldowns.";
	}
}
