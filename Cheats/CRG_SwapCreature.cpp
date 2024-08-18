#include "stdafx.h"
#include "CRG_SwapCreature.h"
#include <Spore/Simulator/cCreatureGameData.h>
#include <Spore/Simulator/cSpeciesProfile.h>

CRG_SwapCreature::CRG_SwapCreature()
{
}


CRG_SwapCreature::~CRG_SwapCreature()
{
}


void CRG_SwapCreature::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsCreatureGame() && GameNounManager.GetAvatar()) {
		auto request = Sporepedia::ShopperRequest(this);
		request.shopperID = id("creatureshopper");
		request.Show(request);
	}
}

void CRG_SwapCreature::OnShopperAccept(const ResourceKey& selection)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->mSpeciesKey = selection;
		avatar->SetModelKey(selection);
		avatar->SetHasModelChanged(true);
		avatar->SetScale(6);
		GameNounManager.UpdateModels();
		GameNounManager.mpAvatar->mSpeciesKey = selection;
	}
}

const char* CRG_SwapCreature::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Allows choosing a new creature from the sporepedia to use as the avatar species.";
	}
	else {
		return "SwapCreature: Allows choosing a new creature from the sporepedia to use as the avatar species.";
	}
}
