#pragma once

#include <Spore\BasicIncludes.h>

#define CRG_MovementHelperPtr intrusive_ptr<CRG_MovementHelper>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;
using namespace App;

class CRG_MovementHelper 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("CRG_MovementHelper");
	
	CRG_MovementHelper();
	~CRG_MovementHelper();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;
	//-----------------------------------------------
	int mbMovementInputTotal = 0;

	bool CanRun() const;
	void UpdateMovingFromKey(int vkey, bool keydown);
	void TestForStuckMovement();

	//-----------------------------------------------
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
};
