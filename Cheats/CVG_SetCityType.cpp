#include "stdafx.h"
#include "CVG_SetCityType.h"
#include "../Common.h"

CVG_SetCityType::CVG_SetCityType()
{
}


CVG_SetCityType::~CVG_SetCityType()
{
}


void CVG_SetCityType::ParseLine(const ArgScript::Line& line)
{
	mParameter = line.GetArguments(1)[0];
	int value = clamp(mpFormatParser->ParseInt(mParameter), 0, 4);

	auto city = Common::GetNearestCity();
	if (city) {
		city->mVehicleSpecialty = value;
	}
}

const char* CVG_SetCityType::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets the vehicle type of the nearest city.\n0 : Military\n1 : Religious\n2 : Economic\n4 : Colony (will crash in civ)";
	}
	else {
		return "SetCityType: Sets the vehicle type of the nearest city.\n0 : Military\n1 : Religious\n2 : Economic\n4 : Colony (will crash in civ)";
	}
}
