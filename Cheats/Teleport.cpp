#include "stdafx.h"
#include "Teleport.h"

Teleport::Teleport()
{
}


Teleport::~Teleport()
{
}


void Teleport::ParseLine(const ArgScript::Line& line)
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
	if (avatar) {
		auto mousepos = GameViewManager.GetWorldMousePosition(0, avatar.get());
		if (mousepos != Vector3(0.0f, 0.0f, 0.0f)) {
			avatar->Teleport(mousepos, avatar->GetOrientation());
		}
	}

	else if (Simulator::IsTribeGame()) {
		auto tribe = GameNounManager.GetPlayerTribe();
		if (!tribe) { return; }
		auto chief = tribe->GetLeaderCitizen();
		if (chief) {
			auto mousepos = GameViewManager.GetWorldMousePosition(0, chief);
			if (mousepos != Vector3(0.0f, 0.0f, 0.0f)) {
				chief->Teleport(mousepos, chief->GetOrientation());
			}
		}
	}
}

const char* Teleport::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Teleports the avatar to the mouse position on the ground.";
	}
	else {
		return "Teleport: Teleports the avatar to the mouse position on the ground.";
	}
}
