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

//------------------------------------------------------------------

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
//// Auto-Called Actions

//-----
// UI

bool cTribeToolStrategy::RightClickedTool(cTribeToolPtr tool) {
	if (mbToolStateValid) {
		TribeToolManager.SelectedUseTool(tool);
	}
	return false;
}

bool cTribeToolStrategy::LeftClickedTool(cTribeToolPtr tool) {
	return false;
}

//-------------
// Creature

void cTribeToolStrategy::CreatureHandheldItemChanged(cCreatureCitizenPtr citizen, cTribeToolPtr tool) {
}

void cTribeToolStrategy::CreatureAcquiredTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool) {
	SporeDebugPrint("Creature %ls used tool type %i", citizen->mCreatureName.c_str(), tool->mToolType);
}

//------------------------------------------------------------------
//// User-Called Actions

/*
cTribeToolManager::ToolMetadata* cTribeToolStrategy::GetToolMetadata(cTribeToolPtr tool) const {
	return TribeToolManager.GetTribeToolMetadata(tool);
}*/

void cTribeToolStrategy::SetToolState(bool state) {
	mbToolStateValid = state;
}

bool cTribeToolStrategy::GetToolState() const {
	return mbToolStateValid;
}

uint32_t cTribeToolStrategy::GetInteractAnim(cCreatureCitizenPtr citizen, cTribeToolPtr tool) const {
	auto meta = TribeToolManager.GetTribeToolMetadata(tool);
	if (meta->mToolInteractAnim != 0x0) {
		return meta->mToolInteractAnim;
	}
	return 0x03CDACC0; // get_tool_shed
}

//------------------------------------------------------------------
//// Messages

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