#include "stdafx.h"
#include "TRG_AddToTribe.h"

TRG_AddToTribe::TRG_AddToTribe()
{
}


TRG_AddToTribe::~TRG_AddToTribe()
{
}


void TRG_AddToTribe::ParseLine(const ArgScript::Line& line)
{
	if (!IsTribeGame()) { return; }

	auto playerTribe = GameNounManager.GetPlayerTribe();
	for (auto creature : GetData<cCreatureCitizen>()) {
		if (creature->IsRolledOver()) {
			AddToTribe(creature, playerTribe);
		}
	}
}

void TRG_AddToTribe::AddToTribe(cCreatureCitizenPtr creature, cTribe* tribe) {
	// only convert creature the target tribe doesn't already own
	if (creature && tribe && creature->mpOwnerTribe != tribe) {

		creature->mpOwnerTribe = tribe;
		creature->SetPoliticalID(tribe->GetPoliticalID());

		tribe->mTribeMembers.push_back(creature);
		tribe->mSelectableMembers.push_back(creature);
	}
}


const char* TRG_AddToTribe::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Adds the hovered creature to your tribe.";
	}
	else {
		return "AddToTribe: Adds the hovered creature to your tribe.";
	}
}
