#include "stdafx.h"
#include "SetEmpireColor.h"

using namespace Simulator;

SetEmpireColor::SetEmpireColor()
{
}


SetEmpireColor::~SetEmpireColor()
{
}


void SetEmpireColor::ParseLine(const ArgScript::Line& line)
{
	auto mParam1 = line.GetArguments(1)[0];
	ColorRGB color = mpFormatParser->ParseColorRGB(mParam1);
	ColorRGBA colorRGBA = ColorRGBA(color.r, color.g, color.b, 1.0f);
	uint32_t colorHex = uint32_t(round(color.r*0xFF)*0x10000 + round(color.g*0xFF)*0x100 + round(color.b*0xFF));


	// TODO: none of these are fully working.
	if (IsTribeGame()) {
		auto tribe = GameNounManager.GetPlayerTribe();
		if (!tribe) { return; }
		tribe->mCachedColor = color;
		tribe->mIDColorID = colorHex;
		tribe->GetHut()->GetModel()->SetColor(colorRGBA);
		for (auto tool : tribe->mTools) {
			tool->GetModel()->SetColor(colorRGBA);
		}
	}
	else if (IsCivGame()) {
		auto civilization = GameNounManager.GetPlayerCivilization();
		auto cities = GetData<cCity>();

		if (!civilization) { return; }
		civilization->mCachedColor = color;
		civilization->mPrimaryColor = color;
		civilization->mIDColorID = colorHex;
		for (auto city : cities) {
			if (city->IsPlayerOwned()) {
				city->mpCityWalls->GetModel()->SetColor(colorRGBA);
				// set turrets
				for (size_t i = 0; i < city->mTurrets.size(); i+= 1) {
					city->mTurrets[i]->GetModel()->SetColor(colorRGBA);
					// debug: minimum flags to apply ID color.
					//city->mTurrets[i]->GetModel()->SetUseColorAsIdentity(true);
				}
			}
		}
	}
	else if (IsSpaceGame()) {
		// Not working
		cEmpire* empire = Simulator::GetPlayerEmpire();
		cPlanetRecord* planetRecord = Simulator::GetActivePlanetRecord();

		auto civilization = GameNounManager.GetPlayerCivilization();
		auto cities = GetData<cCity>();

		if (civilization) {
			civilization->mCachedColor = color;
			civilization->mPrimaryColor = color;
			civilization->mIDColorID = colorHex;
		}
		
		empire->mCachedColor = color;
		empire->mIDColorID = colorHex;
		empire->UpdateAndGetColor();

		if (planetRecord) {
			for (auto city : GetData<cCity>()) {
				if (city->IsPlayerOwned()) {
					//TODO: re-run the color setting with a delay so it actually works
					city->mpCityWalls->GetModel()->SetUseColorAsIdentity(true);
					city->mpCityWalls->GetModel()->SetColor(colorRGBA);
				}
			}
		}
	}
}

const char* SetEmpireColor::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets the color of your empire, tribe, or civilization. Color should be formatted as r,g,b (NOT FULLY WORKING)";
	}
	else {
		return "SetEmpireColor: Sets the color of your empire, tribe, or civilization. Color should be formatted as r,g,b (NOT FULLY WORKING)";
	}
}
