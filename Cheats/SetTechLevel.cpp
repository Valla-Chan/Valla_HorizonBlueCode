#include "stdafx.h"
#include "SetTechLevel.h"

SetTechLevel::SetTechLevel()
{
}


SetTechLevel::~SetTechLevel()
{
}


void SetTechLevel::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsCivGame()) {
		auto civ = GameNounManager.GetPlayerCivilization();

		mParameter = line.GetArguments(1)[0];
		civ->mVehicleTechLevel = mpFormatParser->ParseInt(mParameter);

		switch (civ->mVehicleTechLevel) {
			case 0: App::ConsolePrintF("Civilization tech level set to no vehicles."); break;
			case 1: App::ConsolePrintF("Civilization tech level set to land vehicles."); break;
			case 2: App::ConsolePrintF("Civilization tech level set to land and water vehicles."); break;
			case 3: App::ConsolePrintF("Civilization tech level set to land, water, and air vehicles."); break;
		}
	}
}

const char* SetTechLevel::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets the vehicle technology level of your civilization. Takes an integet argument from 0-3, with 0 allowing no vehicle types, and 3 allowing all vehicle types up to air.";
	}
	else {
		return "SetTechLevel: Sets the vehicle technology level of your civilization. Takes an integet argument from 0-3, with 0 allowing no vehicle types, and 3 allowing all vehicle types up to air.";
	}
}
