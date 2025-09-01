#pragma once

#include <Spore\BasicIncludes.h>

#define CommonPtr intrusive_ptr<Common>
using namespace Simulator;

// Common functions used in multiple places

namespace Common 
{
	// Grox key
	const ResourceKey kGrox = ResourceKey(0x06577404, TypeIDs::Names::crt, 0x40626200);


	static float GetDistance(Vector3 point1, Vector3 point2) {
		return abs((point1 - point2).Length());
	}

	// Get camera pos
	static Vector3 GetCameraPos() {
		auto pViewer = CameraManager.GetViewer();
		Vector3 cameraPos;
		{
			Vector3 cameraDir;
			pViewer->GetCameraToMouse(cameraPos, cameraDir);
			return cameraPos;
		}
		return Vector3();
	}

	// Is the camera within this range of the planet
	static bool IsCameraInPlanetRange(float camera_max_dist = 700) {
		auto pViewer = CameraManager.GetViewer();
		// get camera distance from planet
		float cameraDist = 0;
		{
			Vector3 cameraPos;
			Vector3 cameraDir;
			pViewer->GetCameraToMouse(cameraPos, cameraDir);
			cameraDist = cameraPos.Length();
		}
		// detect camera dist, below ~700 should suffice for close range.
		if (cameraDist < camera_max_dist) {
			return true;
		}
		return false;
	}

	// Get city nearest to camera
	static cCityPtr GetNearestCity() {
		return PlanetModel.GetNearestCity(GetCameraPos());
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
