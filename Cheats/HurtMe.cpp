#include "stdafx.h"
#include "HurtMe.h"

HurtMe::HurtMe()
{
}


HurtMe::~HurtMe()
{
}


void HurtMe::ParseLine(const ArgScript::Line& line)
{
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		mParameter = line.GetArguments(1)[0];
		avatar->mHealthPoints -= mpFormatParser->ParseFloat(mParameter);
	}
	else if (Simulator::IsSpaceGame())
	{
		auto ship = Simulator::GetPlayerUFO();
		if (ship) {
			ship->mHealthPoints -= mpFormatParser->ParseFloat(mParameter);
		}
	}
	
}

const char* HurtMe::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Deals the desired amount of damage to the avatar creature.";
	}
	else {
		return "HurtMe: Deals the desired amount of damage to the avatar creature.";
	}
}
