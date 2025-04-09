#include "stdafx.h"
#include "ToolFrenzyBomb.h"

using namespace Simulator;

ToolFrenzyBomb::ToolFrenzyBomb()
{
}


ToolFrenzyBomb::~ToolFrenzyBomb()
{
}

bool ToolFrenzyBomb::OnHit(cSpaceToolData* pTool, const Vector3& position, SpaceToolHit hitType, int unk) {
	auto inventory = SimulatorSpaceGame.GetPlayerInventory();
	bool success = cDefaultProjectileWeapon::OnHit(pTool, position, hitType, unk);

	pTool->mDamageRadius = 150;
	pTool->mDamageArea = 0;
	//pTool.dam

	// do not allow an air or water hit type
	if (hitType < 2) {
		/*
		IVisualEffectPtr effect;
		if (EffectsManager.CreateVisualEffect(0x03fd5261, 0, effect)) {
		 effect->SetRigidTransform(Transform()
		  .SetOffset(position)
		  .SetScale(1.1f));
		 effect->Start();
		}*/

		auto creatures = GetDataByCast<cCreatureBase>();
		for (size_t i = 0; i < creatures.size(); i++) {

			// TODO: make sure you cannot use this on allies.
			if (creatures[i]->mPoliticalID != GetPlayerEmpire()->mPoliticalID) {

				// Out of range, return.
				if (Math::distance(creatures[i]->GetPosition(), position) > pTool->mDamageRadius) { continue; }

				// find nearest target
				auto target = GetNearestFriendlyCombatant(creatures[i]);
				if (!target) { return success; }

				auto creature = object_cast<cCreatureBase>(creatures[i]);
				if (creature) {

					creature->mMaxHealthPoints = 100;
					creature->SetHealthPoints(creature->mMaxHealthPoints);
					
					creature->SetTarget(target.get());
					creature->mpCombatantTarget = target.get();
					creature->SetCreatureTarget(target.get(),true, 3);
					creature->mpLastAttacker = target;
					creature->mLastAttacker = target->GetPoliticalID();
					creature->PlayAbility(creature->mDefaultAttackAbilityIndex);
					//creature->TakeDamage(1, target->GetPoliticalID(), 1, target->ToSpatialObject()->GetOrientation().ToEuler(), target.get());
					auto citizen = object_cast<cCreatureCitizen>(creature);
					if (citizen) {
						citizen->SetPoliticalID(0x01);
						citizen->DoAction(kCitizenActionAttack, target->ToGameData());
					}
					continue;
				}
			}
		}

		pTool->mDamageRadius;
	}

	return false;
}

cCombatantPtr ToolFrenzyBomb::GetNearestFriendlyCombatant(cCreatureBasePtr ref, TargetPref targetpref) {
	cCombatantPtr closest_combatant = nullptr;
	float closest_distance = float(INFINITE);
	
	// For animals, select a creature from their herd
	auto animal = object_cast<cCreatureAnimal>(ref);
	if (animal) {
		animal = animal;
		/*
		for (size_t i = 0; i < animal->mHerd->; i++) {

		}
		animal->mHerd*/
	}
	// For Citizens, select a target from their own city.
	auto citizen = object_cast<cCreatureCitizen>(ref);
	if (citizen) {
		switch (targetpref) {
			case Creature:
				if (citizen->mpOwnerCity) {
					auto citizens = GetDataByCast<cCreatureCitizen>();
					for (size_t i = 0; i < citizens.size(); i++) {
						if (citizens[i]->mpOwnerCity == citizen->mpOwnerCity) {
							UpdateCombatantFromDistance(
								citizens[i], ref,
								closest_distance, closest_combatant);
						}
						
					}
				}
				else if (citizen->mpOwnerTribe) {
					for (size_t i = 0; i < citizen->mpOwnerTribe->mCommunityMembers.size(); i++) {
						UpdateCombatantFromDistance(
							citizen->mpOwnerTribe->mCommunityMembers[i], ref,
							closest_distance, closest_combatant);
					}
				}
				
				break;
			case Building:
				break;
		}

	}
	return closest_combatant;
}

float ToolFrenzyBomb::Distance(cCombatantPtr target, cCombatantPtr ref) const {
	return Math::distance(target->ToSpatialObject()->GetPosition(), ref->ToSpatialObject()->GetPosition());
}

void ToolFrenzyBomb::UpdateCombatantFromDistance(cCombatantPtr target, cCombatantPtr ref, float &cdist, cCombatantPtr &ccombatant) {
	auto dist = Distance(target, ref);
	if (dist < cdist) {
		cdist = dist;
		ccombatant = target;
	}
}

/*
bool ToolFrenzyBomb::OnSelect(Simulator::cSpaceToolData* pTool)
{
 return false;
}*/