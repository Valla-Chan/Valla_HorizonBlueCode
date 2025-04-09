#include "stdafx.h"
#include "CRG_GiveAllParts.h"

CRG_GiveAllParts::CRG_GiveAllParts()
{
}


CRG_GiveAllParts::~CRG_GiveAllParts()
{
}

using namespace Simulator;
void CRG_GiveAllParts::ParseLine(const ArgScript::Line& line)
{
	App::ConsolePrintF("Unlocking all parts, please wait...");
	App::ScheduleTask(this, &CRG_GiveAllParts::UnlockParts, 0.25f);
}

void CRG_GiveAllParts::UnlockParts() {
	if (!IsCreatureGame() || !GameNounManager.GetAvatar()) { return; }

	auto partcount = GetPlayer()->mpCRGItems->mUnlockableItems.size();
	//auto size2 = GetPlayer()->mpCRGItems->mUnlockedItems.size();
	//size_t partcount = GetPlayer()->mpCRGItems->mUnlockableItems.size() - GetPlayer()->mpCRGItems->mUnlockedItems.size();
	partcount = clamp(int(partcount), 1, 9000);
	for (size_t i = 0; i < partcount; i++) {
		App::CreatureModeStrategies::UnlockPart action = { GameNounManager.GetAvatar(), 0, 4 };
		CreatureModeStrategy.ExecuteAction(action.ID, &action);
	}
}

const char* CRG_GiveAllParts::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Unlocks all available creature parts. WARNING: SLOW!";
	}
	else {
		return "GiveAllParts: Unlocks all available creature parts. WARNING: SLOW!";
	}
}
