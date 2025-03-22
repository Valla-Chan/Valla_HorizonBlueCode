#include "stdafx.h"
#include "CVG_CityWalls.h"

CVG_CityWalls::CVG_CityWalls()
{
}


CVG_CityWalls::~CVG_CityWalls()
{
}

// For internal use, do not modify.
int CVG_CityWalls::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CVG_CityWalls::Release()
{
	return DefaultRefCounted::Release();
}

cCityPtr CVG_CityWalls::GetCurrentCity() {
	auto planetRecord = Simulator::GetActivePlanetRecord();
	auto planet = PlanetModel.Get();
	if (!planet) { return nullptr; }

	// get camera pos
	Vector3 camPos;
	Vector3 camDir;
	CameraManager.GetViewer()->GetCameraToMouse(camPos, camDir);


	return planet->GetNearestCity(camPos);
}

// new city wall has been hovered; store the current city item data in preparation to upgrade.
void CVG_CityWalls::StoreCityData() {
	auto city = GetCurrentCity();

	//slotted_buildings.clear();
	//slotted_decor.clear();
	//slotted_turrets.clear();
	/*
	for (auto slot : city->mBuildingsLayout.mSlots) {
		if (slot.mIsOccupied) {
			slotted_buildings.push_back(slot.mpObject);
		}
	}*/
	/*
	for (auto slot : city->mDecorationsLayout.mSlots) {
		if (slot.mIsOccupied) {
			slotted_decor.push_back(slot.mpObject);
		}
	}*/
	/*
	for (auto slot : city->mTurretsLayout.mSlots) {
		if (slot.mIsOccupied) {
			slotted_turrets.push_back(slot.mpObject);
		}
	}*/


}

// city walls have decreased in size, remove all buildings and decor not in a slot.
void CVG_CityWalls::ProcessCityWallReduction() {
	auto city = GetCurrentCity();

	// Buildings
	for (auto building : GetDataByCast<cBuilding>()) {
		// run only on non-cityhall buildings in this city
		if (building && building->GetOwnerCity() == city && !object_cast<cBuildingCityHall>(building)) {
			// check if slotted
			bool slotted = false;
			for (auto slot : city->mBuildingsLayout.mSlots) {
				if (slot.mIsOccupied && slot.mpObject == building) {
					slotted = true;
					break;
				}
			}
			if (!slotted) {
				// Remove and refund the building.
				city->mpCivilization->mWealth += building->GetCost();
				city->RemoveBuilding(building.get());
				GameNounManager.DestroyInstance(building.get());
				
			}
		}
	}

	/*
	// Decor
	for (auto decor : slotted_decor) {
		// check if slotted
		bool slotted = false;
		for (auto slot : city->mDecorationsLayout.mSlots) {
			if (slot.mIsOccupied && slot.mpObject == decor) {
				slotted = true;
				break;
			}
		}
		if (!slotted) {
			city->RemoveBuilding(building.get());
		}
	}*/

	city->ProcessBuildingUpdate(city.get(), 0, 0);
}

// The method that receives the message. The first thing you should do is checking what ID sent this message...
bool CVG_CityWalls::HandleMessage(uint32_t messageID, void* message)
{
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return true;
}
