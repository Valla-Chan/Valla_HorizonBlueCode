#include "stdafx.h"
#include "GetTribeColors.h"

GetTribeColors::GetTribeColors()
{
}


GetTribeColors::~GetTribeColors()
{
}


void GetTribeColors::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsTribeGame()) {
		for (auto tribe : Simulator::GetDataByCast<Simulator::cTribe>()) {
			App::ConsolePrintF("--- Tribe info for 0x%x ---", tribe->GetCommunityName());
			App::ConsolePrintF("Identity Color ID: 0x%x", tribe->mIDColorID);
			App::ConsolePrintF("Identity Cached Color: %x", tribe->mCachedColor);
			auto chieftain = tribe->GetLeaderCitizen();
			if (chieftain) {
				auto color = tribe->GetLeaderCitizen()->GetIdentityColor();
				App::ConsolePrintF("Chieftain Identity Color: (%f, %f, %f)", color.r, color.g, color.b);
				App::ConsolePrintF("Chieftain Color as Identity?: %b", tribe->GetLeaderCitizen()->mbColorIsIdentity);
			}
			App::ConsolePrintF("------------------------");
		}
	}
}

const char* GetTribeColors::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Prints the colors of all tribes.";
	}
	else {
		return "GetTribeColors: Prints the colors of all tribes.";
	}
}
