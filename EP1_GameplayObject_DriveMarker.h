#pragma once

#include <Spore\BasicIncludes.h>

#define EP1_GameplayObject_DriveMarkerPtr intrusive_ptr<EP1_GameplayObject_DriveMarker>

using namespace UTFWin;
using namespace Simulator;

class EP1_GameplayObject_DriveMarker 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObject_DriveMarker");
	
	EP1_GameplayObject_DriveMarker();
	~EP1_GameplayObject_DriveMarker();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;
	

	Vector4 mVecInputRaw = Vector4(0, 0, 0, 0); // left, right, down, up
	Vector2 mVecInput = Vector2(0, 0);
	Vector3 mDestinationPos = Vector3(0,0,0); // if (0,0) then do not drive there.

	static bool IsPlayingAdventure();
	static cVehiclePtr GetHoveredVehicle();
	static cCreatureAnimalPtr GetHoveredCreature();
	bool IsVehicleDrivable(cVehiclePtr vehicle) const;
	float GetCurrentVehicleSpeed() const;
	cSpatialObjectPtr GetVehicleSpatial() const;

	void EnterVehicle(cVehiclePtr vehicle);
	void EnterCreature(cCreatureAnimalPtr creature);
	void Enter();
	void ExitVehicle();
	void ExitVehicle_Delayed();
	void StartDrivingCooldown();
	void EndDrivingCooldown();

	bool UpdateVecFromKey(int vkey, bool keydown);
	void SetDestinationFromCursor();
	void SetDestinationFromInput();
	void DriveToDestination();
	void TeleportAvatarToVehicle();
	void OrientVehicleCamera();
	void CheckAndStop();

	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
private:
	// List of all drivable vehicles
	eastl::vector<cVehiclePtr> mDrivableVehicles = {};
	// The vehicle we might be driving
	cVehiclePtr mCurrentVehicle;
	// The creature we might be riding
	cCreatureAnimalPtr mCurrentCreature;

	
	bool mIsDriving; // True if the user is "inside" the vehicle, false otherwise
	bool mDrivingCooldown = false; // true if in cooldown from driving or exiting
	bool mIsMouseHeld = false;
	bool mIsMiddleMouseHeld = false;

	float mPrevAvatarScale = 1.0f;
	float mPrevAvatarHealth = 0.0f;

	intrusive_ptr<Simulator::ScheduledTaskListener> mTask;
	
};
