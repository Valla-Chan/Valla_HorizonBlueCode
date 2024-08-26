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
	cTribePtr tribe = Simulator::GetDataByCast<Simulator::cTribe>()[0];
	tribe->mIDColorID = Simulator::IdentityColors::Yellow;
	//tribe->GetLeaderCitizen()->SetIdentityColor(ColorRGB(1.0f, 0.8f, 0.0f));
	tribe->AssignNames(tribe->GetLeaderCitizen()->mCreatureName);
}

const char* SetChieftainColor::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets Chieftain color to Yellow.";
	}
	else {
		return "SetChieftainColor: Sets Chieftain color to Yellow.";
	}
}
