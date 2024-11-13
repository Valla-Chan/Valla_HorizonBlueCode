#include "stdafx.h"
#include "SetChieftainColor.h"

SetChieftainColor::SetChieftainColor()
{
}


SetChieftainColor::~SetChieftainColor()
{
}


void SetChieftainColor::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsTribeGame()) {
		for (auto tribe : Simulator::GetData<Simulator::cTribe>()) {
			tribe->mIDColorID = (int)Simulator::IdentityColors::Red;
			auto chieftain = tribe->GetLeaderCitizen();
			if (chieftain) {
				bool tribeIsPlayer = tribe == GameNounManager.GetPlayerTribe();
				chieftain->mbColorIsIdentity = true;
				chieftain->SetIdentityColor(ColorRGB(1.0f, 0.0f, 0.0f));
				tribe->mIDColorID = 0x053dbcf9;
			}
		}
	}
	//tribe->AssignNames(tribe->GetLeaderCitizen()->mCreatureName);
}

const char* SetChieftainColor::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets all Chieftain colors to Red.";
	}
	else {
		return "SetChieftainColor: Sets all Chieftain colors to Red.";
	}
}
