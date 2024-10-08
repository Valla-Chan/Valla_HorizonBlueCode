#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator.h>

#define TRG_CreaturePickupPtr intrusive_ptr<TRG_CreaturePickup>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;

class TRG_CreaturePickup 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("TRG_CreaturePickup");
	
	TRG_CreaturePickup();
	~TRG_CreaturePickup();
	void Update() override;

	bool temp_deny_pickup = false;

	cCreatureCitizenPtr possible_member;
	cCreatureCitizenPtr held_member;

	static cTribePtr GetPlayerTribe();
	void DeselectMembers();
	void Pickup(cCreatureCitizenPtr creature);
	void Drop();
	void Moved();

	void ClickPosseUI(cCreatureBasePtr creature);
	void UnclickMB1();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;
	
};
