#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\CommonIDs.h>
#include "Common.h"
#include "CityMemberManager.h"


#define cCreaturePickupPtr intrusive_ptr<cCreaturePickup>
#define CreaturePickupManager (*cCreaturePickup::Get())

using namespace UTFWin;
using namespace Simulator;
using namespace Common;

class cCreaturePickup 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("cCreaturePickup");
	static const uint32_t cur_grab_closed = 0x03C32077;

	cCreaturePickup();
	~cCreaturePickup();
	int AddRef() override { return DefaultRefCounted::AddRef(); }
	int Release() override { return DefaultRefCounted::Release(); }
	void* Cast(uint32_t type) const override;
	static cCreaturePickup* Get();
	void Update() override;
	//------------------------------

	static cTribePtr GetPlayerTribe() { return GameNounManager.mpPlayerTribe; };
	bool IsValidGameMode() const;
	bool IsCreatureHeld() const { return mpHeldMember != nullptr; }
	bool CanPickUp(cSpatialObjectPtr target) const;

	void DeselectMembers();
	void SetPossibleMember();
	void Pickup(cSpatialObjectPtr target);
	void Drop();
	void Moved();

	void UnclickMB1(const Message& refmessage);

	// Return the hovered spatial object if it is valid for pickup, otherwise return nullptr
	cSpatialObjectPtr GetPickupObject() const;
	// Return a list of spatial objects that can be picked up
	//vector<cSpatialObjectPtr> GetSelectableMembers() const;

	//------------------------------

	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;

protected:
	static cCreaturePickup* sInstance;

private:
	enum PICKUP_FLAGS {
		NONE = 0,
		ALLOW_TRIBE = 1,
		ALLOW_CITIZEN = 2,
		ALLOW_ENEMY = 4,
		ALLOW_ANIMAL = 8,
		ALLOW_VEHICLE = 16,
		ALLOW_PET = 32,
		ALLOW_EPIC = 32,
		ALLOW_ALL = 128,
	};

	//-----------------------------

	// TODO: in space stage, creatures should be dropped at a certain height, between mode switches (in/out of ship), and when exiting atmosphere.
	// Also consider making the grab function be a "stasis beam", where you use a tool to grab and release objects.
	vector<uint32_t> VALID_GAME_MODES = {
		GameModeIDs::kGameTribe,
		GameModeIDs::kGameCiv,
		GameModeIDs::kGameSpace,
	};

	uint32_t VALID_PICKUP_FLAGS =
		ALLOW_TRIBE |
		ALLOW_CITIZEN |
		ALLOW_ENEMY |
		ALLOW_ANIMAL |
		ALLOW_VEHICLE |
		ALLOW_PET |
		ALLOW_EPIC |
		ALLOW_ALL |
		0;

	bool mbTempDenyPickup = false;
	cSpatialObjectPtr mpPossibleMember = nullptr;
	cSpatialObjectPtr mpHeldMember = nullptr;
};


// Detour the cursor setting func
static bool CreaturePickup_SetCursor_detour(UTFWin::cCursorManager* obj, uint32_t& id) {
	if (cCreaturePickup::Get() && CreaturePickupManager.IsValidGameMode()) {

		//// Only run these if not in the planner
		// ---------------------------------------
		if (!Common::IsPlannerOpen()) {
			// held tribe member
			if (CreaturePickupManager.IsCreatureHeld()) {
				id = CreaturePickupManager.cur_grab_closed;
				return true;
			}
			else if (CreaturePickupManager.GetPickupObject()) {
				id = BasicCursorIDs::Cursors::GrabOpen;
				return true;
			}
		}
	}
	return false;
}