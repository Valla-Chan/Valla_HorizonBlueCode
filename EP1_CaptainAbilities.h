#pragma once

#include <Spore\BasicIncludes.h>

#define EP1_CaptainAbilitiesPtr intrusive_ptr<EP1_CaptainAbilities>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;

class EP1_CaptainAbilities 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("EP1_CaptainAbilities");
	
	EP1_CaptainAbilities();
	~EP1_CaptainAbilities();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

	int jumpcount = 0;
	bool can_jumpburst = false;

	bool CanDoubleJump(cCreatureAnimalPtr creature, int max_jumps) const;
	bool DoubleJump(float fwd_speed, int max_jumps, int energy);

	
	void StartBurstResetTimer();
	void ResetAllowJumpBurst();
	void ResetJumpCount();


	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

private:
	// This task can be stopped and reset to a new time
	intrusive_ptr<Simulator::ScheduledTaskListener> mJumpReset;
};
