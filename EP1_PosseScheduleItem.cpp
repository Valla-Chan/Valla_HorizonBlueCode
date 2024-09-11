#include "stdafx.h"
#include "EP1_PosseScheduleItem.h"

EP1_PosseScheduleItem::EP1_PosseScheduleItem()
{
}


EP1_PosseScheduleItem::~EP1_PosseScheduleItem()
{
	if (mTask && !mTask->HasExecuted()) {
		Simulator::RemoveScheduledTask(mTask);
	}
}


void EP1_PosseScheduleItem::UpdateScheduleTime(float time) {
	if (mTask && !mTask->HasExecuted()) {
		Simulator::RemoveScheduledTask(mTask);
	}
	ScheduleFinished();
	if (time > 0.0f) {
		//saved_behavior = mCreature->field_B4C;
		//mCreature->field_B4C = 0;
		//mTask = nullptr;
		mTask = Simulator::ScheduleTask(this, &EP1_PosseScheduleItem::ScheduleFinished, time);
		mbCanMove = false;
		SporeDebugPrint("Scheduling task to end in %f seconds.", time);
	}

	
}


void EP1_PosseScheduleItem::ScheduleFinished() {
	mbCanMove = true;
	//if (!saved_behavior) {
	//	mCreature->field_B4C = saved_behavior;
	//}
	SporeDebugPrint("Schedule has finished.");
}



// For internal use, do not modify.
int EP1_PosseScheduleItem::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EP1_PosseScheduleItem::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_PosseScheduleItem::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(EP1_PosseScheduleItem);
	return nullptr;
}
