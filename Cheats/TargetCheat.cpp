#include "stdafx.h"
#include "TargetCheat.h"


cCreatureBasePtr TargetCheat::GetAvatarCreature() {
	if (GameNounManager.GetAvatar()) {
		return { GameNounManager.GetAvatar() };
	}
	else if (Simulator::IsTribeGame()) {
		cTribePtr tribe = GameNounManager.mpPlayerTribe;
		return tribe->GetLeaderCitizen();
	}
	return nullptr;
}

cCombatantPtr TargetCheat::GetAvatarCombatant() {
	cCombatantPtr avatar = GetAvatarCreature();
	if (avatar) { return avatar; }
	else if (IsSpaceGame()) {
		return Simulator::GetPlayerUFO();
	}
	return nullptr;
}

vector<cCreatureBasePtr> TargetCheat::GetCreatureTargets(bool get_combat_targets) {
	vector<cCreatureBasePtr> targets = {};

	if (Simulator::IsTribeGame()) {
		auto tribes = Simulator::GetData<Simulator::cTribe>();
		for (auto tribe : tribes) {
			eastl::vector<cCreatureCitizenPtr>& members = tribe->GetSelectableMembers();
			for (auto member : members) {
				if (member->IsSelected()) {
					if (get_combat_targets && member->GetAnimalTarget()) {
						targets.push_back(member->GetAnimalTarget());
					}
					else {
						targets.push_back(member);
					}

				}
			}
		}
	}

	return targets;
}

vector<cCombatantPtr> TargetCheat::GetCombatantTargets(bool get_combat_targets) {
	vector<cCombatantPtr> targets = {};

	// Get player's combat target if get_combat_targets
	if (get_combat_targets) {
		auto avatar = GetAvatarCombatant();
		if (avatar) {
			auto target = avatar->GetTarget();
			if (target) {
				targets.push_back(target);
			}
		}
	}

	// Find selected animals in CRG
	if (Simulator::IsCreatureGame()) {
		auto animals = Simulator::GetData<Simulator::cCreatureAnimal>();
		for (auto animal : animals) {
			if (animal->IsSelected()) {
				targets.push_back(animal);
			}
		}
	}
	// Find selected citizens in TRG, and their combat targets if get_combat_targets
	if (Simulator::IsTribeGame()) {
		auto tribes = Simulator::GetData<Simulator::cTribe>();
		for (auto tribe : tribes) {
			eastl::vector<cCreatureCitizenPtr>& members = tribe->GetSelectableMembers();
			for (auto member : members) {
				if (member->IsSelected()) {
					if (get_combat_targets && member->GetAnimalTarget()) {
						targets.push_back(member->GetAnimalTarget());
					}
					else {
						targets.push_back(member);
					}

				}
			}
		}
	}
	// Find selected vehicles in CVG
	else if (Simulator::IsCivGame()) {
		auto vehicles = GetData<cVehicle>();
		for (auto vehicle : vehicles) {
			if (vehicle->IsSelected()) {
				targets.push_back(vehicle);
			}
		}
	}

	return targets;
}
