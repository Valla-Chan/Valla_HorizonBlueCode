#include "stdafx.h"
#include "Kill.h"

Kill::Kill()
{
}


Kill::~Kill()
{
}


void Kill::ParseLine(const ArgScript::Line& line)
{
	auto targets = GetCombatantTargets(true);
	if (targets.size() > 0) {
		for (auto target : targets) {
			KillTarget(target);
		}
	}
	else {
		cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
		if (avatar) {
			auto target = avatar->GetTarget();
			if (target) {
				target->SetHealthPoints(0);
			}
			else {
				KillTarget(avatar);
			}
		}
		else if (Simulator::IsSpaceGame())
		{
			auto ship = Simulator::GetPlayerUFO();
			if (ship) {
				ship->mHealthPoints = 0;
			}
		}
		else if (Simulator::IsCellGame()) {
			auto avatarcell = Simulator::Cell::GetPlayerCell();
			if (avatarcell) {
				avatarcell->mHealthPoints = 0;
			}
		}
	}
}

void Kill::KillTarget(const cCombatantPtr target) {
	auto crt_target = object_cast<Simulator::cCreatureBase>(target);
	if (crt_target) {
		if (Simulator::IsCreatureGame()) {
			crt_target->mEnergy = -90.0f;
			crt_target->mHunger = -90.0f;
			crt_target->SetHealthPoints(0.0001f);
		}
	}
	target->SetHealthPoints(0.0f);
}


const char* Kill::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Kills the targeted creature instantly, or kills the avatar creature if nothing is selected.";
	}
	else {
		return "Kill: Kills the targeted creature instantly, or kills the avatar creature if nothing is selected.";
	}
}
