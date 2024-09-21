#include "stdafx.h"
#include "TRG_GetTribeInfo.h"

TRG_GetTribeInfo::TRG_GetTribeInfo()
{
}


TRG_GetTribeInfo::~TRG_GetTribeInfo()
{
}


void TRG_GetTribeInfo::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsTribeGame()) {
		cTribePtr tribe = GameNounManager.GetPlayerTribe();
		App::ConsolePrintF("--- Tribe info for 0x%x ---", tribe->GetCommunityName());
		App::ConsolePrintF("Identity Color: %x",tribe->mIDColorID);
		App::ConsolePrintF("Tribe Upgrade level: %x",tribe->mUpgradeLevel);
		App::ConsolePrintF("Population size: %x",tribe->GetPopulationCount());
		App::ConsolePrintF("Population capacity: %x",tribe->mCommunityMembers.capacity());
		App::ConsolePrintF("Tribe Zone Radius: %x",tribe->mZoningRadius);
		App::ConsolePrintF("Chieftain: 0x%x", tribe->GetLeaderCitizen()->mCreatureName);
		App::ConsolePrintF("Chieftain Color: 0x%x", tribe->GetLeaderCitizen()->GetIdentityColor());
		App::ConsolePrintF("Tribe Members: %x", tribe->GetTribeMembers().size());
		for (auto creature : tribe->GetTribeMembers()) {
			App::ConsolePrintF("	0x%x", creature->mCreatureName);
		}
		App::ConsolePrintF("----------------------------");
		App::ConsolePrintF("Tribe Layout: 0x%x",tribe->field_3B4);
		App::ConsolePrintF("Total Food: %i",tribe->GetTotalFood());
		App::ConsolePrintF("Food in Egg Pen: %x",tribe->mEggPenFoodValue);
		App::ConsolePrintF("Tools Count: %x",tribe->mPurchasedTools);
		App::ConsolePrintF("----------------------------");
		auto tools = Simulator::GetData<Simulator::cTribeTool>();
		int toolnum = 0;
		for (auto tool : tools) {
			toolnum += 1;
			App::ConsolePrintF("Tool %i data below:", toolnum);
			App::ConsolePrintF("	mToolType:  %i", tool->mToolType);
			App::ConsolePrintF("	field_1F0:  %i", tool->field_1F0);
			App::ConsolePrintF("	field_1F4:  %i", tool->field_1F4);
			App::ConsolePrintF("	field_1F8:  %i", tool->field_1F8);
			App::ConsolePrintF("	field_240:  %i", tool->field_240);
			App::ConsolePrintF("	field_244:  %i", tool->field_244);
			App::ConsolePrintF("	field_1EC:  %f", tool->field_1EC);
		}
	}
	
}

const char* TRG_GetTribeInfo::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Prints debug info about the player's tribe.";
	}
	else {
		return "TribeInfo: EPrints debug info about the player's tribe.";
	}
}
