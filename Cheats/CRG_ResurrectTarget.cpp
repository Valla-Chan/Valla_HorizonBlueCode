#include "stdafx.h"
#include "CRG_ResurrectTarget.h"
#include <Spore/Simulator/cCreatureGameData.h>

CRG_ResurrectTarget::CRG_ResurrectTarget()
{
}


CRG_ResurrectTarget::~CRG_ResurrectTarget()
{
}



void CRG_ResurrectTarget::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		cCreatureAnimalPtr target = avatar->GetAnimalTarget();
		if (target) {
			target->mbDead = false;
			target->mHerd->mbExtinction = false;
			target->mbHasBeenEaten = false;
			if (target->mMaxHealthPoints < 2) {
				target->mMaxHealthPoints = 15;
			}
			target->SetHealthPoints(target->mMaxHealthPoints);
			target->mHunger = 100.0;
			target->func90h();
		}
		else {
			App::ConsolePrintF("This cheat requires a valid target!");
		}
	}
}

const char* CRG_ResurrectTarget::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat resurrects the targeted dead creature.";
	}
	else {
		return "Resurrect: This cheat resurrects the targeted dead creature.";
	}
}
