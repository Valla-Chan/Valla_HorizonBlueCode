#include "stdafx.h"
#include "TribeToolStratManager.h"

cTribeToolStratManager::cTribeToolStratManager()
{
	sInstance = this;
	AddBaseStrategies();
	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, SimulatorMessages::kMsgSwitchGameMode);
}


cTribeToolStratManager::~cTribeToolStratManager()
{
	sInstance = nullptr;
}

cTribeToolStratManager* cTribeToolStratManager::sInstance;

cTribeToolStratManager* cTribeToolStratManager::Get()
{
	return sInstance;
}

// For internal use, do not modify.
int cTribeToolStratManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cTribeToolStratManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cTribeToolStratManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(cTribeToolStratManager);
	return nullptr;
}

//-------------------------------------------------------------------------------

void cTribeToolStratManager::AddStrategy(cTribeToolStrategy* pStrategy, uint32_t strategyID)
{
	mStrategies[strategyID] = pStrategy;
}

cTribeToolStrategyPtr cTribeToolStratManager::GetStrategy(uint32_t strategyID)
{
	auto E = mStrategies.find(strategyID);
	if (E != mStrategies.end()) {
		return E.get_node()->mValue.second;
	}
	return nullptr;
}

cTribeToolStrategyPtr cTribeToolStratManager::GetStrategyForToolType(int toolID)
{
	auto E = mTribeToolStrategies.find(toolID);
	if (E != mTribeToolStrategies.end()) {
		return GetStrategy(E.get_node()->mValue.second);
	}
	return nullptr;
}

void cTribeToolStratManager::SetToolIDStrategy(int toolID, uint32_t strategyID) {
	auto strat = GetStrategy(strategyID);
	if (strat) {
		mTribeToolStrategies[toolID] = strategyID;
		strat->AddHandledToolID(toolID);
	}
}



//-------------------------------------------------------------------------------


bool cTribeToolStratManager::HandleMessage(uint32_t messageID, void* msg)
{
	if (messageID == kMsgSwitchGameMode) {
		mMouseButtonsPressed.Reset();
	}
	return false;
}

int cTribeToolStratManager::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

bool cTribeToolStratManager::HandleUIMessage(IWindow* window, const Message& message)
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
// NOTE: this (tool strat manager) handles the inputs, and defer the hovered tool functions to the strategies that handle that tool.

// TODO: replace the right clicking in this for a detour of the tool input strategy
bool cTribeToolStratManager::HandleToolMouseMessage(IWindow* window, const Message& message)
{
	if (message.IsType(kMsgMouseDown) || message.IsType(kMsgMouseUp) || message.IsType(kMsgMouseMove)) {

		auto hoveredTool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());

		// Mouse Clicked
		if (message.IsType(kMsgMouseDown)) {
			// New Click, reset buttons and interrupt state
			if (!mMouseButtonsPressed.AreAnyPressed()) {
				mMouseButtonsPressed.Reset();
				mpStoredTool = hoveredTool;
			}
			mMouseButtonsPressed.SetState(message.Mouse.mouseButton, true);
		}
		// Mouse Released
		else if (message.IsType(kMsgMouseUp)) {
			auto handled = false;
			
			if (!mMouseButtonsPressed.interrupted && hoveredTool && mpStoredTool == hoveredTool) {
				// Find the strategy that handles this tool, and pass the click inputs to it.
				auto toolID = mpStoredTool->GetToolType();
				auto strat = GetStrategyForToolType(toolID);
				// R
				if (mMouseButtonsPressed.IsRightMousePressed()) {
					handled = strat->RightClickedTool(mpStoredTool);
				}
				// L
				else if (mMouseButtonsPressed.IsLeftMousePressed()) {
					handled = strat->LeftClickedTool(mpStoredTool);
				}
				
			}
			mMouseButtonsPressed.SetState(message.Mouse.mouseButton, false);
			//mMouseButtonsPressed.Clear();
			return handled;
		}
		// Mouse Moved
		else if (message.IsType(kMsgMouseMove) && mMouseButtonsPressed.AreAnyPressed()) {
			mMouseButtonsPressed.Interrupt();
		}
	}

	return false;
}