#include "stdafx.h"
#include "SetTime.h"
#include <Spore\Simulator\SubSystem\SpacePlayerData.h>
#include <Spore\Simulator\cTimeOfDay.h>

SetTime::SetTime()
{
}


SetTime::~SetTime()
{
}


void SetTime::ParseLine(const ArgScript::Line& line)
{
	if (!(Simulator::IsCreatureGame() || Simulator::IsSpaceGame() || Simulator::IsCivGame() || Simulator::IsTribeGame() || Simulator::IsScenarioMode() )) { return; }
	// cancel if not in planet view
	if (Simulator::IsSpaceGame()) {
		if (SpacePlayerData::Get()->mCurrentContext != Simulator::SpaceContext::Planet) { return; }
	}

	mParameter = line.GetArguments(1)[0];
	float time = clamp(mpFormatParser->ParseFloat(mParameter), 0.0f, 24.0f);

	Vector3 position = Vector3(1, 1, 1);
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		position = avatar->GetPosition();
	}
	else {
		Vector3 camPos = Vector3(0, 0, 0);
		Vector3 camDir = Vector3(0, 0, 0);

		// Get pos on planet from center of the screen.
		auto pViewer = CameraManager.GetViewer();
		auto windowArea = WindowManager.GetMainWindow()->GetArea();
		pViewer->GetCameraToPoint(windowArea.right / 2.0f, windowArea.bottom / 2.0f, camPos, camDir);

		if (camPos != Vector3(0, 0, 0)) {
			position = camPos;
		}
		else {
			if (Simulator::IsTribeGame()) {
				position = GameNounManager.GetPlayerTribe()->GetPosition();
			}
			//else if (Simulator::IsCivGame()) {
			//	position = GameNounManager.Get
			//}
		}
	}

	float timepercent = time * 40.0f;
	TimeOfDay.SetTimeOfDay(timepercent, position);
	App::ConsolePrintF("Time set to %i:00", int(time));
}

const char* SetTime::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets the time of the day by hour. Uses 24 hour time.";
	}
	else {
		return "TimeSet: Sets the time of the day by hour. Uses 24 hour time.";
	}
}
