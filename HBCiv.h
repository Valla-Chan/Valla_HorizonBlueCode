#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\SubSystem\CommManager.h>
#include "Common.h"
// CVG
#include "HBCommManager.h"
#include "CityWallsManager.h"
#include "CityMemberManager.h"


#define HBCivPtr intrusive_ptr<HBCiv>

using namespace UTFWin;
using namespace BasicCursorIDs;

// CVG

static CityWallsManager* cvg_citywalls;

class HBCiv
{

public:

	static void Initialize() {
		auto cvg_membermanager = new(cCityMemberManager);
		auto hbcommmanager = new(cHBCommManager);
		cvg_citywalls = new(CityWallsManager);
	}
	static void Dispose() {
		cvg_citywalls = nullptr;
	}

	static void AttachDetours();
};

//----------------
// Detour Helpers

// Detour the tribe spawning func
static bool CVG_TribeSpawn_detour(const Math::Vector3& position, int& tribeArchetype, int& numMembers, int& foodAmount, bool& boolvalue, cSpeciesProfile* species) {
	if (IsCivGame()) {
		species = CityMemberManager.GetRandomTribeSpecies(species);
		return true;
	}
	return false;
}

static bool CVG_LocalStringSetText_detour(LocalizedString* obj, uint32_t& tableID, uint32_t& instanceID) {
	// Hack to fix some civ-type-specific interactions in HB
	// TODO: make a proper way to interact with or replace CNV actions and fix their flags
	if (tableID == id("civ_convo_hb")) {
		VehiclePurpose playertype = CityMemberManager.GetCivMajorityType(GameNounManager.GetPlayerCivilization());
		switch (instanceID) {

			case 0x0000C0A1: // player deny NPC surrender
				switch (playertype) {
				case kVehicleMilitary:
					instanceID = 0x0000C0B1; break;
				case kVehicleEconomic:
					instanceID = 0x0000C0C1; break;
					return true;
				}
			case 0x000FCAA1: // player response to Religious NPC's never surrender
				if (playertype == kVehicleMilitary) { instanceID = 0x000FCAB1; }
				else if (playertype != kVehicleCultural) { instanceID = 0x000FCFF1; }
				return true;

			case 0x000FCBB1: // player response to Military NPC's never surrender
				if (playertype == kVehicleCultural) { instanceID = 0x000FCAA1; return true; }
				else if (playertype != kVehicleMilitary) { instanceID = 0x000FCFF1; }
				return true;
		}
	}
	return false;
}

static void NewCityAppeared() {
	CityMemberManager.NewCityAppeared();
}
// Detour the UIEventLog ShowEvent func
static bool CVG_UIShowEvent_detour(cUIEventLog* obj, uint32_t& instanceID, uint32_t& groupID)
{
	// New city has appeared in civ
	if (IsCivGame() && instanceID == id("NewCityAppears")) {
		Simulator::ScheduleTask(NewCityAppeared, 0.000000001f);
	}
}

// Detour the cursor setting func
static bool CVG_SetCursor_detour(UTFWin::cCursorManager* obj, uint32_t& id) {
	// fix the deny spice geyser cursor issue
	if (IsCivGame() && id == Cursors::NoOptions) {
		auto hovered = GameViewManager.GetHoveredObject();
		if (hovered) {
			cCommodityNodePtr geyser = object_cast<cCommodityNode>(hovered);
			if (geyser && geyser->mPoliticalID != GameNounManager.GetPlayerCivilization()->GetPoliticalID()) {
				id = Cursors::ClaimSpice;
				return true;
			}
		}
	}

	return false;
}

// PaletteUI::SetActiveCategory
static void CVG_PaletteUISetActiveCategory_detour(int& categoryIndex) {
	if (IsCivGame()) {
		// TODO: start tracking if the player mouses over city wall up/downgrades
		cvg_citywalls->ProcessCityWallReduction();
		MessageManager.MessageSend(id("ResetTimescale"), nullptr);
	}
}

//----------
// Detours

// Spawn vehicle
member_detour(CitySpawnVehicle_detour, Simulator::cCity, cVehicle* (VehiclePurpose speciality, VehicleLocomotion locomotion, struct ResourceKey key, bool isSpaceStage)) {
	cVehicle* detoured(VehiclePurpose speciality, VehicleLocomotion locomotion, struct ResourceKey key, bool isSpaceStage) {
		auto vehicle = original_function(this, speciality, locomotion, key, isSpaceStage);
		App::ScheduleTask(this, &CitySpawnVehicle_detour::SetVehicleScale, 0.0001f);

		return vehicle;
	}

	void CitySpawnVehicle_detour::SetVehicleScale() {
		auto vehicles = GetDataByCast<cVehicle>();
		if (vehicles.size() == 0) { return; }
		auto vehicle = vehicles[vehicles.size() - 1];
		if (vehicle) {
			vehicle->SetScale(vehicle->mScale * 1.7f);
		}
	}
};

// Update city building color
static_detour(CityProcessBuildingUpdate_detour, void(cCity*, int, int)) {
	void detoured(cCity * city, int int1, int int2) {
		original_function(city, int1, int2);
		if (cvg_citywalls) {
			cvg_citywalls->UpdateCityBuildingColor(city);
		}
	}

};

// Detour GetCachedColorFromId
static_detour(GetCachedColorFromId_detour, const Math::ColorRGB& (uint32_t)) {
	Math::ColorRGB IDColor;
	const Math::ColorRGB& detoured(uint32_t colorId) {
		// If the color ID has only 6 digits, read it as an RGB value.
		if (colorId <= 0x00FFFFFF) {
			IDColor = Color(colorId);
			return IDColor;
		}
		return original_function(colorId);
	}
};

//-----------------
// ATTACH DETOURS
void HBCiv::AttachDetours() {
	cHBCommManager::AttachDetours();
	CitySpawnVehicle_detour::attach(GetAddress(Simulator::cCity, SpawnVehicle));
	CityProcessBuildingUpdate_detour::attach(GetAddress(Simulator::cCity, ProcessBuildingUpdate));
	GetCachedColorFromId_detour::attach(GetAddress(Simulator, GetCachedColorFromId));
}
