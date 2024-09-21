#include "stdafx.h"
#include "SetTimeScale.h"

SetTimeScale::SetTimeScale()
{
}


SetTimeScale::~SetTimeScale()
{
}


void SetTimeScale::ParseLine(const ArgScript::Line& line)
{
	mParameter = line.GetArguments(1)[0];
	float speed = mpFormatParser->ParseFloat(mParameter);

	if (speed <= 0.0f || abs(speed - 1.0f) < 0.1f  ) {
		speed = 1.0f;
	}
	GameTimeManager.SetSpeedFactors(speed, 2.0f, 4.0f, 8.0f);

	GameTimeManager.SetSpeed(0);
}

const char* SetTimeScale::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets the game speed to this value. 1 is default; smaller values decrease speed, and higher values increase it.";
	}
	else {
		return "Timescale: Sets the game speed to this value. 1 is default; smaller values decrease speed, and higher values increase it.";
	}
}
