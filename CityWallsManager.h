#pragma once

#include <Spore\BasicIncludes.h>

#define CityWallsManagerPtr intrusive_ptr<CityWallsManager>

using namespace Simulator;
class CityWallsManager 
	: public App::IMessageListener
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("CityWallsManager");

	CityWallsManager();
	~CityWallsManager();

	int AddRef() override;
	int Release() override;

	const float bld_glow_scale = 2.3f;
	const float bld_glow_add = 0.3f;

	// store the slotted objects of the last city, when hovering over a city wall up/down grade.
	//vector<cBuildingPtr> slotted_buildings = {};
	//vector<cOrnamentPtr> slotted_decor = {};
	//vector<cTurretPtr> slotted_turrets = {};

	float GetScaledGlow(float value);
	void UpdateCityBuildingColor(cCityPtr city);
	void SetCityBuildingColor(cCityPtr city, ColorRGB color, bool glow = true);

	void StoreCityData();
	void ProcessCityWallReduction();
	
	// This is the function you have to implement, called when a message you registered to is sent.
	bool HandleMessage(uint32_t messageID, void* message) override;
};
