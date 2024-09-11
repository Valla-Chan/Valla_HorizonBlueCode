#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\AI\cBehaviorTreeData.h>

#define EP1_PosseScheduleItemPtr intrusive_ptr<EP1_PosseScheduleItem>

class EP1_PosseScheduleItem 
	: public Object
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("EP1_PosseScheduleItem");
	
	EP1_PosseScheduleItem();
	~EP1_PosseScheduleItem();

	//cBehaviorTreeData* saved_behavior = 0;

	cCreatureAnimalPtr mCreature;
	bool mbCanMove;

	void UpdateScheduleTime(float time);
	void RemoveFromPosse();
	void ScheduleFinished();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

private:
	// This task can be stopped and reset to a new time
	intrusive_ptr<Simulator::ScheduledTaskListener> mTask;
};
