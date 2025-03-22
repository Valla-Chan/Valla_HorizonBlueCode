#include "stdafx.h"
#include "HurtTarget.h"

HurtTarget::HurtTarget()
{
}


HurtTarget::~HurtTarget()
{
}



cCreatureCitizenPtr HurtTarget::TraceHitCitizen() const {

	auto pViewer = CameraManager.GetViewer();

	Vector3 camPos = Vector3(0, 0, 0);
	Vector3 camDir = Vector3(0, 0, 0);

	// Get vector to the center of the screen.
	auto windowArea = WindowManager.GetMainWindow()->GetArea();
	pViewer->GetCameraToMouse(camPos, camDir);

	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto citizen = object_cast<Simulator::cCreatureCitizen>(object);
			if (citizen && citizen->mpOwnerCity->IsPlayerOwned()) {
				return citizen;
			}
		}
	}
	return nullptr;
}

void HurtTarget::ParseLine(const ArgScript::Line& line)
{

	// NOTE: the city rollover is VERY aggressive and supercedes the creatures' rollover.
	// If the city popup is open, we need to run a manual traceline in order to get a creature to rollover.

	auto window = WindowManager.GetMainWindow();
	auto cityHover = window->FindWindowByID(0x0199A7DE); // find the city hover UI

	cCombatantPtr hovered = nullptr;

	// Player-owned city hovered
	if (cityHover && cityHover->IsVisible()) {
		hovered = TraceHitCitizen();
	}
	// No city hovered, fall back to actual rollover
	if (!hovered && GameViewManager.mHoveredObject) {
		hovered = object_cast<Simulator::cCombatant>(GameViewManager.mHoveredObject);
	}
	if (!hovered) {
		App::ConsolePrintF("A valid combatant must be hovered to use this cheat.");
		return;
	}

	// Deal damage
	mParameter = line.GetArguments(1)[0];
	hovered->TakeDamage( max(0.0f, mpFormatParser->ParseFloat(mParameter)), 0x0, 0, Vector3(0, 0, 0), nullptr);
}

const char* HurtTarget::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Deals the desired amount of damage to the hovered combatant.";
	}
	else {
		return "HurtMe: Deals the desired amount of damage to the hovered combatant.";
	}
}
