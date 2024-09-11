#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator.h>
#include "EP1_PosseScheduleItem.h"

#define EP1_PosseCommandPtr intrusive_ptr<EP1_PosseCommand>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;

class EP1_PosseCommand 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("EP1_PosseCommand");
	
	EP1_PosseCommand();
	~EP1_PosseCommand();

	eastl::vector<EP1_PosseScheduleItem*> mPosseSchedules = {}; // schedules of creatures currently being commanded.

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	EP1_PosseScheduleItem* GetCreatureSchedule(cCreatureAnimalPtr creature);
	int FindScheduledCreature(cCreatureAnimalPtr creature);

	void UpdateCreatureSchedule(cCreatureAnimalPtr creature, float time);
	void CommandPosse();

	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
};
