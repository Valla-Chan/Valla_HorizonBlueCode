#include "stdafx.h"
#include "Kill.h"

Kill::Kill()
{
}


Kill::~Kill()
{
}


void Kill::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->mEnergy = -90.0f;
		avatar->mHunger = -90.0f;
		avatar->SetHealthPoints(0.0001f);
	}
}



const char* Kill::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Kills the avatar creature instantly.";
	}
	else {
		return "Kill: Kills the avatar creature instantly.";
	}
}
