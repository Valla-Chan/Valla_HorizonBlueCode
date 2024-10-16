#include "stdafx.h"
#include "SetGravity.h"

SetGravity::SetGravity()
{
}


SetGravity::~SetGravity()
{
}


void SetGravity::ParseLine(const ArgScript::Line& line)
{
	// TODO.
	//PlanetModel.GetGravity
}

const char* SetGravity::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Set's the current planet's gravity.";
	}
	else {
		return "SetGravity: Set's the current planet's gravity.";
	}
}
