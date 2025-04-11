#include "stdafx.h"
#include "EP1_PosseCommand.h"
//#include "Common.h"

EP1_PosseCommand::EP1_PosseCommand()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
}


EP1_PosseCommand::~EP1_PosseCommand()
{
}


int EP1_PosseCommand::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

//-----------------------------------------------------------------------------------------------

EP1_PosseScheduleItem* EP1_PosseCommand::GetCreatureSchedule(cCreatureAnimalPtr creature) {
	size_t idx = FindScheduledCreature(creature);
	if (idx != -1 && idx < mPosseSchedules.size()) {
		return mPosseSchedules[idx];
	}
	return nullptr;
}

// If a creature is in the schedule item list, return the index of the schedule item.
// Also do memory cleanup.
int EP1_PosseCommand::FindScheduledCreature(cCreatureAnimalPtr creature) {
	size_t empty = 0;
	for (size_t i = 0; i < mPosseSchedules.size(); i++) {
		// if an entry is null, note that.
		if (mPosseSchedules[i] == nullptr) {
			empty++;
		}
		// list is empty, can be cleared.
		if (empty >= mPosseSchedules.size() - 1) {
			mPosseSchedules.clear();
			//mPosseSchedules.reset_lose_memory();
			return -1;
		}

		// found a match! return index
		else if (mPosseSchedules[i]->mCreature == creature) {
			return int(i);
		}

	}
	return -1;
}

// Find a creature and update its schedule item, or add an item if one does not exist.
void EP1_PosseCommand::UpdateCreatureSchedule(cCreatureAnimalPtr creature, float time) {
	int idx = FindScheduledCreature(creature);
	// found
	if (idx > -1) {
		mPosseSchedules[idx]->UpdateScheduleTime(time);
		SporeDebugPrint(" %    Updating existing schedule.", time);
	}
	// did not find, add new item.
	else {
		auto scheduleItem = new(EP1_PosseScheduleItem);
		mPosseSchedules.push_back(scheduleItem);
		scheduleItem->mCreature = creature;
		scheduleItem->UpdateScheduleTime(time);
		SporeDebugPrint(" %   Creating new schedule.", time);
	}

}

void EP1_PosseCommand::CommandPosse() {
	// For each posse member, move to clicked location.
	auto avatar = GameNounManager.GetAvatar();
	for (auto creature : GameNounManager.mPosseMembers) {
		if (avatar->GetAnimalTarget() == creature || creature->IsSelected()) {

			auto pos = GameViewManager.GetWorldMousePosition(0, object_cast<cCombatant>(creature));
			auto dist = abs((creature->GetPosition()- pos).Length());
			auto time = dist / creature->GetStandardSpeed();

			SporeDebugPrint("------------------------------------------------");
			SporeDebugPrint("trying to move posse to mouse pos %f, %f, %f", pos.x, pos.y, pos.z);
			SporeDebugPrint("creature info: speed: %f  distance from goal: %f", creature->GetStandardSpeed(), dist);
			SporeDebugPrint("time to reach: %f", time);
			SporeDebugPrint("------------------------------------------------");


			// force the creature out of their current anim and go to pos
			creature->mpAnimatedCreature->PlayAnimation(0x00BF10B3);
			creature->WalkTo(1,pos, pos,1.0f, 1.0f);

			// add creature to list
			UpdateCreatureSchedule(creature, time);
			

			// TODO: play tribal stage "mhm!" voice line.
			//creature->PlayVoice("", 2, 0);
			
			// TODO: play goto-location or circle-select ground effect

		}
	}
}


//-----------------------------------------------------------------------------------------------

bool EP1_PosseCommand::HandleUIMessage(IWindow* window, const Message& message)
{
	if ( !((IsScenarioMode() && ScenarioMode.GetMode() != App::cScenarioMode::Mode::EditMode) || IsCreatureGame()) ) { return false; }

	// Player has left clicked the mouse with CTRL held
	if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseDown) && (message.Mouse.mouseState & MouseStateFlags::kMouseCtrlDown) != 0) { // 
		CommandPosse();

		// This may need to return false if posse is to be commanded towards a selected object.
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------


// For internal use, do not modify.
int EP1_PosseCommand::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EP1_PosseCommand::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_PosseCommand::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(EP1_PosseCommand);
	return nullptr;
}