#include "stdafx.h"
#include "cTribeToolStrategy.h"
#include "TribeToolManager.h"

cTribeToolStrategy::cTribeToolStrategy()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
	App::AddUpdateFunction(this);
}


cTribeToolStrategy::~cTribeToolStrategy()
{
}

// For internal use, do not modify.
int cTribeToolStrategy::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cTribeToolStrategy::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cTribeToolStrategy::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(cTribeToolStrategy);
	return nullptr;
}

void cTribeToolStrategy::Update()
{
}

void cTribeToolStrategy::AddHandledToolID(int id) {
	for (int item : mHandledToolIDs) {
		if (item == id) {
			return;
		}
	}
	mHandledToolIDs.push_back(id);
}


bool cTribeToolStrategy::HandlesToolID(int id) const {
	if (mbHandleAllTools) {
		return true;
	}
	for (int item : mHandledToolIDs) {
		if (item == id) {
			return true;
		}
	}
	return false;
}

bool cTribeToolStrategy::HandlesTool(cTribeToolPtr tool) const {
	return HandlesToolID(tool->GetToolType());
}

//------------------------------------------------------------------

bool cTribeToolStrategy::RightClickedTool(cTribeToolPtr tool) {
	TribeToolManager.SelectedUseTool(tool);
	return false;
}

bool cTribeToolStrategy::LeftClickedTool(cTribeToolPtr tool) {
	return false;
}

//------------------------------------------------------------------

void cTribeToolStrategy::CreatureHandheldItemChanged(cCreatureCitizenPtr citizen) {
	if (HandlesToolID(citizen->mSpecializedTool)) {
		auto tribe = citizen->mpOwnerTribe;
		if (tribe) {
			auto tool = citizen->mpOwnerTribe->GetToolByType(citizen->mSpecializedTool);
			CreatureAcquiredTool(citizen, tool);
		}
	}
}

// TODO: Called when the creature's handheld object changes
void cTribeToolStrategy::CreatureAcquiredTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool) {
}


//------------------------------------------------------------------


bool cTribeToolStrategy::HandleMessage(uint32_t messageID, void* msg)
{
	return false;
}

int cTribeToolStrategy::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

bool cTribeToolStrategy::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsTribeGame()) {
		return false;
	}
	
	return false;
}