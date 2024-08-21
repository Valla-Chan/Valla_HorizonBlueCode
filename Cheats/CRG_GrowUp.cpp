#include "stdafx.h"
#include "CRG_GrowUp.h"

CRG_GrowUp::CRG_GrowUp()
{
}


CRG_GrowUp::~CRG_GrowUp()
{
}


void CRG_GrowUp::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		cCreatureAnimalPtr target = avatar->GetAnimalTarget();
		if (target) {
			target->mAge += target->mAge;
			target->SetScale(target->CalculateScale(false));
		}
	}
}

const char* CRG_GrowUp::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Ages up the targetted creature into an adult.";
	}
	else {
		return "GrowUp: Ages up the targetted creature into an adult.";
	}
}
