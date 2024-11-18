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
				if (!tribeIsPlayer) {
					auto flags = chieftain->GetModel()->mFlags;
					auto model = chieftain->mpAnimatedCreature->GetModel();
					model->mFlags = 770051;
					//SporeDebugPrint("NPC FLAGS %x, %i", flags, flags);
				}
				if (tribeIsPlayer) {
					auto flags = chieftain->GetModel()->mFlags;
					auto model = chieftain->mpAnimatedCreature->GetModel();
					model->mFlags = 770053;
					//SporeDebugPrint("PLAYER FLAGS %x, %i, %b", flags, flags, chieftain->mbColorIsIdentity);
				}
				chieftain->mbColorIsIdentity = true;
				chieftain->SetIdentityColor(ColorRGB(1.0f, 0.0f, 0.0f));
				
				tribe->mIDColorID = 0xFF5522;
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
