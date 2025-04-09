#include "stdafx.h"
#include "cTribeToolStrategy.h"

cTribeToolStrategy::cTribeToolStrategy()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, SimulatorMessages::kMsgSwitchGameMode);
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

cTribeToolPtr cTribeToolStrategy::GetHoveredTool(bool playerOwned) {
	auto hoveredTool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());
	if (playerOwned) {
		if (hoveredTool && hoveredTool->mTribe == GameNounManager.GetPlayerTribe()) {
			return hoveredTool;
		}
		else {
			return nullptr;
		}
	}
	return hoveredTool;
}

// TODO: may need to access tribe tool metadata.
bool cTribeToolStrategy::CanToolBeUsed(cTribeToolPtr tool) {
	return true;
	//tool->data
}

vector<cCreatureCitizenPtr> cTribeToolStrategy::GetSelectedMembers() {
	vector<cCreatureCitizenPtr> selected;
	auto tribe = GameNounManager.GetPlayerTribe();
	eastl::vector<cSpatialObjectPtr>& members = tribe->GetSelectableMembers();
	for (auto member : members) {
		auto citizen = object_cast<Simulator::cCreatureCitizen>(member);
		if (member->IsSelected() && citizen) {
			selected.push_back(citizen);
		}
	}
	return selected;
}

void cTribeToolStrategy::UseTool(vector<cCreatureCitizenPtr> citizens, cTribeToolPtr tool) {
	for (size_t i = 0; i < citizens.size(); i++) {
		UseTool(citizens[i], tool);
	}
}

void cTribeToolStrategy::UseTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool) {
	citizen->DoAction(kCitizenActionGrabTool, tool->ToGameData());
}

/// Tell the selected citizen creatures to use a tool.
void cTribeToolStrategy::SelectedUseTool(cTribeToolPtr tool) {
	if (CanToolBeUsed(tool)) {
		auto selected = GetSelectedMembers();
		if (selected.size() > 0) {
			UseTool(selected, tool);
		}
	}
}

//------------------------------------------------------------------

bool cTribeToolStrategy::RightClickedTool(cTribeToolPtr tool) {
	SelectedUseTool(tool);
	return false;
}

bool cTribeToolStrategy::LeftClickedTool(cTribeToolPtr tool) {
	return false;
}

//------------------------------------------------------------------

void cTribeToolStrategy::TryCreatureAcquiredTool(cCreatureCitizenPtr citizen) {
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

// TODO: needs metadata access
uint32_t cTribeToolStrategy::GetGrabOverrideAnim() const {
	return 0x0;
}

//------------------------------------------------------------------

bool cTribeToolStrategy::HandleMessage(uint32_t messageID, void* msg)
{
	if (messageID == kMsgSwitchGameMode) {
		mMouseButtonsPressed.Reset();
	}
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

	if (HandleToolMouseMessage(window, message)) {
		return true;
	}
	
	return false;
}

// handles clicking, right clicking, mouse interruptions, etc.
bool cTribeToolStrategy::HandleToolMouseMessage(IWindow* window, const Message& message)
{
	if (message.IsType(kMsgMouseDown) || message.IsType(kMsgMouseUp) || message.IsType(kMsgMouseMove)) {

		auto hoveredTool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());
		//if (!hoveredTool) { return false; }

		// Mouse Clicked
		if (message.IsType(kMsgMouseDown)) {
			// New Click, reset buttons and interrupt state
			if (!mMouseButtonsPressed.AreAnyPressed()) {
				mMouseButtonsPressed.Reset();
				mpStoredObject = hoveredTool;
			}
			mMouseButtonsPressed.SetState(message.Mouse.mouseButton, true);
		}
		// Mouse Released
		else if (message.IsType(kMsgMouseUp)) {
			auto handled = false;
			mMouseButtonsPressed.SetState(message.Mouse.mouseButton, false);
			if (!mMouseButtonsPressed.interrupted && hoveredTool && mpStoredObject == hoveredTool && HandlesTool(hoveredTool)) {
				// R
				if (mMouseButtonsPressed.IsRightMousePressed()) {
					handled = RightClickedTool(mpStoredObject);
				}
				// L
				else if (mMouseButtonsPressed.IsLeftMousePressed()) {
					handled = LeftClickedTool(mpStoredObject);
				}
				return handled;
			}
			mMouseButtonsPressed.Clear();
		}
		// Mouse Moved
		else if (message.IsType(kMsgMouseMove) && mMouseButtonsPressed.AreAnyPressed()) {
			mMouseButtonsPressed.Interrupt();
		}
	}
	
	return false;
}
