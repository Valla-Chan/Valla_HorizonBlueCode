#include "stdafx.h"
#include "CityWallsManager.h"
#include "Common.h"

using namespace Common;

CityWallsManager::CityWallsManager()
{
	MessageManager.AddListener(this, id("PlayerCitySpawned"));
	MessageManager.AddListener(this, id("SporepediaClosed"));
}


CityWallsManager::~CityWallsManager()
{
}

// For internal use, do not modify.
int CityWallsManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CityWallsManager::Release()
{
	return DefaultRefCounted::Release();
}


// scaled color glow amount
// TODO: fix crash on shutdown here?
float CityWallsManager::GetScaledGlow(float value) {
	return (value * bld_glow_scale) + bld_glow_add;
}

void CityWallsManager::UpdateCityBuildingColor(cCityPtr city) {
	if (!city) {return;}
	if (city->mpCivilization) {
		SetCityBuildingColor(city, city->mpCivilization->mCachedColor);
		ColorRGBA colorA = ColorRGBA(city->mpCivilization->mCachedColor.r, city->mpCivilization->mCachedColor.g, city->mpCivilization->mCachedColor.b, 1.0f);
		if (city->mpCityWalls && city->mpCityWalls->GetModel()) {
			city->mpCityWalls->GetModel()->SetColor(colorA);
		}
	}
	// TODO: add in a fix for space stage. mpCivilization is invalid in spg
}

// color all the building windows to this color.
void CityWallsManager::SetCityBuildingColor(cCityPtr city, ColorRGB color, bool glow) {

	ColorRGBA colorA;
	if (glow) {
		colorA = ColorRGBA(GetScaledGlow(color.r), GetScaledGlow(color.g), GetScaledGlow(color.b), 1.0f);
	}

	for (auto building : city->mBuildings) {
		auto model = building->GetModel();
		if (model) {
			model->SetUseColorAsIdentity(true);
			model->SetColor(colorA);
		}
		
	}

}

// new city wall has been hovered; store the current city item data in preparation to upgrade.
void CityWallsManager::StoreCityData() {
	auto city = GetNearestCity();

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
void CityWallsManager::ProcessCityWallReduction() {
	auto city = GetNearestCity();

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
bool CityWallsManager::HandleMessage(uint32_t messageID, void* message)
{
	if (!IsCivGame()) { return false; }

	if (messageID == id("PlayerCitySpawned") || id("SporepediaClosed")) {
		auto cities = GetData<cCity>();
		for (auto city : cities) {
			if (city) {
				UpdateCityBuildingColor(city);
				
			}
		}
		
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return false;
}
