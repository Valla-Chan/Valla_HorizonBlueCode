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

	else if (Simulator::IsTribeGame()) {
		auto tribes = Simulator::GetDataByCast<Simulator::cTribe>();
		for (auto tribe : tribes) {
			eastl::vector<cSpatialObjectPtr>& members = tribe->GetSelectableMembers();
			for (auto member : members) {
				auto creature = object_cast<Simulator::cCreatureBase>(member);
				if (member->IsSelected() && creature) {
					if (creature->GetAnimalTarget()) {
						KillTarget(creature->GetAnimalTarget());
					}
					else {
						KillTarget(creature);
					}
					
				}
			}
		}
	}
	else {
		auto avatarcell = Simulator::Cell::GetPlayerCell();
		if (avatarcell) {
			avatarcell->mHealthPoints = 0;
		}
	}



}

void Kill::KillTarget(const cCreatureBasePtr target) {
	if (Simulator::IsCreatureGame() && target == object_cast<Simulator::cCreatureBase>(GameNounManager.GetAvatar())) {
		target->mEnergy = -90.0f;
		target->mHunger = -90.0f;
		target->SetHealthPoints(0.0001f);
	}
	else {
		target->SetHealthPoints(0.0f);
	}
	
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
