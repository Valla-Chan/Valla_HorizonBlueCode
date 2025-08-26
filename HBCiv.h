#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\SubSystem\CommManager.h>
#include "Common.h"
// CVG
#include "CityWallsManager.h"
#include "CityMemberManager.h"


#define HBCivPtr intrusive_ptr<HBCiv>

using namespace UTFWin;
using namespace BasicCursorIDs;

// CVG

static CityWallsManager* cvg_citywalls;
cCommEvent* pLastEvent = nullptr;

class HBCiv
{

public:

	static void Initialize() {
		auto cvg_membermanager = new(cCityMemberManager);
		cvg_citywalls = new(CityWallsManager);
	}
	static void Dispose() {
		cvg_citywalls = nullptr;
		pLastEvent = nullptr;
	}

	static void AttachDetours();
};

//----------
// Detours

// Spui loading/spawning detour
// TODO: NOT CURRENTLY USED
member_detour(CVG_ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey & name, bool arg_4, uint32_t arg_8) {

		//if (IsCivGame()) {
		//	if (name.instanceID == id("Rollover_CivVehicle")) {
		//
		//	}
		//}

		// planner popup
		//if (name.instanceID == id("plannerPalette")) {
		//	MessageManager.MessageSend(id("PlannerPopup"), nullptr);
		//}

		return original_function(this, name, arg_4, arg_8);
	}
};

// Open comms
member_detour(ShowCommEvent_detour, Simulator::cCommManager, void(cCommEvent*)) {
	void detoured(cCommEvent * pEvent) {
		if (!IsCommScreenActive()) {
			// Fix game time scale by sending msg and delaying comms
			MessageManager.MessageSend(id("CinematicBegin"), nullptr);
			pLastEvent = pEvent;
			App::ScheduleTask(this, &ShowCommEvent_detour::ShowLastCommEvent, 0.001f);
		}
		else {
			original_function(this, pEvent);
		}
	}

	void ShowCommEvent_detour::ShowLastCommEvent() {
		original_function(this, pLastEvent);
		pLastEvent = nullptr;
	}
};

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
		cvg_citywalls->UpdateCityBuildingColor(city);
	}

};

// Detour the tribe spawning func
static_detour(CVG_TribeSpawn_detour, cTribe* (const Vector3&, int, int, int, bool, cSpeciesProfile*)) {
	cTribe* detoured(const Math::Vector3 & position, int tribeArchetype, int numMembers, int foodAmount, bool boolvalue, cSpeciesProfile * species) {
		SporeDebugPrint("Tribe Spawned");
		if (IsCivGame()) {
			species = CityMemberManager.GetRandomTribeSpecies(species);
		}
		cTribe* tribe = original_function(position, tribeArchetype, numMembers, foodAmount, boolvalue, species);
		TribePlanManager.trg_hutmanager->SetupNewTribe(tribe);

		return tribe;
	}
};

// Detour the UIEventLog ShowEvent func
member_detour(CVG_UIShowEvent_detour, Simulator::cUIEventLog, uint32_t(uint32_t, uint32_t, int, Math::Vector3*, bool, int))
{
	void NewCityAppeared() {
		CityMemberManager.NewCityAppeared();
	}

	uint32_t detoured(uint32_t instanceID, uint32_t groupID, int int1, Math::Vector3 * vector3, bool dontAllowDuplicates, int int2)
	{
		// fire OG func
		auto value = original_function(this, instanceID, groupID, int1, vector3, dontAllowDuplicates, int2);

		// New city has appeared in civ
		if (IsCivGame() && instanceID == id("NewCityAppears")) {
			//MessageManager.MessageSend(id("NewCityAppeared"), nullptr);
			Simulator::ScheduleTask(this, &CVG_UIShowEvent_detour::NewCityAppeared, 0.000000001f);
		}

		return value;
	}
};

// Detour the cursor setting func
// TODO: Broken? Why is this not getting called?
member_detour(CVG_SetCursor_detour, UTFWin::cCursorManager, bool(uint32_t)) {
	bool detoured(uint32_t id) {
		// fix the deny spice geyser cursor issue
		if (IsCivGame() && id == Cursors::NoOptions) {
			auto hovered = GameViewManager.GetHoveredObject();
			if (hovered) {
				cCommodityNodePtr geyser = object_cast<cCommodityNode>(hovered);
				if (geyser && geyser->mPoliticalID != GameNounManager.GetPlayerCivilization()->GetPoliticalID()) {
					return original_function(this, Cursors::ClaimSpice);
				}
			}
		}

		return original_function(this, id);
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

// PaletteUI::SetActiveCategory
member_detour(CVG_PaletteUISetActiveCategory_detour, Palettes::PaletteUI, void(int)) {
	void detoured(int categoryIndex) {

		// Civ
		if (IsCivGame()) {
			// TODO: start tracking if the player mouses over city wall up/downgrades
			cvg_citywalls->ProcessCityWallReduction();
		}

	}
};

//-----------------
// ATTACH DETOURS
void HBCiv::AttachDetours() {
	//CVG_ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));
	ShowCommEvent_detour::attach(GetAddress(Simulator::cCommManager, ShowCommEvent));
	CitySpawnVehicle_detour::attach(GetAddress(Simulator::cCity, SpawnVehicle));
	CityProcessBuildingUpdate_detour::attach(GetAddress(Simulator::cCity, ProcessBuildingUpdate));
	CVG_UIShowEvent_detour::attach(GetAddress(Simulator::cUIEventLog, ShowEvent));
	CVG_TribeSpawn_detour::attach(GetAddress(Simulator, SpawnNpcTribe));
	CVG_SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));
	GetCachedColorFromId_detour::attach(GetAddress(Simulator, GetCachedColorFromId));
	CVG_PaletteUISetActiveCategory_detour::attach(GetAddress(Palettes::PaletteUI, SetActiveCategory));
}
