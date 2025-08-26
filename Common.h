#pragma once

#include <Spore\BasicIncludes.h>

#define CommonPtr intrusive_ptr<Common>
using namespace Simulator;

// Common functions used in multiple places

namespace Common 
{

	static float GetDistance(Vector3 point1, Vector3 point2) {
		return abs((point1 - point2).Length());
	}

	//--------------------------------------------------
	// CREATURE ANIMATION OWNERS

	static cCreatureCitizen* GetAnimCreatureCitizenOwner(const AnimatedCreaturePtr& animcreature) {
		cCombatantPtr target = nullptr;
		for (auto creature : Simulator::GetData<Simulator::cCreatureCitizen>()) { //loop through all creatures
			if (creature->mpAnimatedCreature.get() == animcreature) //if the current creature is the owner of the AnimatedCreature that triggered the event
			{
				return creature.get(); //set the crt pointer to the current creature
			}
		}
		return nullptr;
	}

	static cCreatureAnimal* GetAnimCreatureOwner(const AnimatedCreaturePtr& animcreature) {
		cCombatantPtr target = nullptr;
		for (auto creature : Simulator::GetData<Simulator::cCreatureAnimal>()) { //loop through all creatures
			if (creature->mpAnimatedCreature.get() == animcreature) //if the current creature is the owner of the AnimatedCreature that triggered the event
			{
				return creature.get(); //set the crt pointer to the current creature
			}
		}
		return nullptr;
	}

	//--------------------------------------------------
	// ABILITIES

	static bool IsAttackAbility(cCreatureAbilityPtr ability) {
		if (ability->mCategory == 1 && ability->mVerbIconGameLayout.instanceID == 0xAE2CBB58) {
			return true;
		}
		return false;
	}

	static bool IsAbilityInRange(cCreatureCitizenPtr caller, cCombatantPtr target, cCreatureAbilityPtr ability) {
		float distance = Math::distance(target->ToSpatialObject()->GetPosition(), caller->ToSpatialObject()->GetPosition());
		auto radiuscaller = caller->ToSpatialObject()->GetBoundingRadius() * 0.9f;
		auto radiustarget = target->ToSpatialObject()->GetBoundingRadius() * 0.9f;
		if (distance >= ability->mRangeMin && distance <= (ability->mRange + radiuscaller + radiustarget)) {
			return true;
		}
		return false;
	}

	//--------------------------------------------------
	// PLANNER

	static bool IsPlannerOpen() {
		if (IsTribeGame() || IsCivGame() || IsSpaceGame()) {
			auto window = WindowManager.GetMainWindow();
			auto plannerUI = window->FindWindowByID(0x93019DBC);

			if (plannerUI && plannerUI->IsVisible()) { return true; }
		}
		return false;
	}


};
