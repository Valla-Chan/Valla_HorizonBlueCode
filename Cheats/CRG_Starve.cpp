#include "stdafx.h"
#include "CRG_Starve.h"

CRG_Starve::CRG_Starve()
{
}


CRG_Starve::~CRG_Starve()
{
}


void CRG_Starve::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsCreatureGame()) {
		cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
		if (avatar) {
			avatar->mHunger = 0.0f;
		}
	}
	else if (Simulator::IsTribeGame()) {
		auto tribe = GameNounManager.GetPlayerTribe();
		if (tribe) {
			bool starved = false;
			for (auto member : tribe->GetTribeMembers()) {
				if (member->IsSelected()) {
					member->mHunger = 0.0f;
					starved = true;
				}
			}
			if (!starved) {
				for (auto member : tribe->GetTribeMembers()) {
					member->mHunger = 0.0f;
				}
			}
		}
	}
	
}

const char* CRG_Starve::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Starves the avatar creature instantly.";
	}
	else {
		return "Starve: Starves the avatar creature instantly, dropping its hunger bar to empty.";
	}
}
