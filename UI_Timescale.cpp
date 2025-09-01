#include "stdafx.h"
#include "UI_Timescale.h"

UI_Timescale::UI_Timescale()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, SimulatorMessages::kMsgSwitchGameMode);
	MessageManager.AddListener(this, id("ResetTimescale"));
}


UI_Timescale::~UI_Timescale()
{
}

//-----------------------------------------------------------------------------

eastl::vector<IWindow*> UI_Timescale::GetTimescaleButtons() {
	if (mTimescaleButtons.size() == 2 && mTimescaleButtons[1]) { return mTimescaleButtons; }
	else {
		mTimescaleButtons.clear();
		auto window = WindowManager.GetMainWindow();
		auto button2x = window->FindWindowByID(Timescale2x);
		auto button2xstop = window->FindWindowByID(Timescale2xStop);
		auto button4x = window->FindWindowByID(Timescale4x);
		auto button4xstop = window->FindWindowByID(Timescale4xStop);
		if (button2x)
		{
			// [0, 1]
			mTimescaleButtons.push_back(button2x);
			mTimescaleButtons.push_back(button4x);
			// [2,3]
			mTimescaleButtons.push_back(button2xstop);
			mTimescaleButtons.push_back(button4xstop);
		}
	}
	return mTimescaleButtons;
}

// TODO: set this back to 0 when a cutscene plays, the stage is left or editor is entered, or when using the tribal planner.
void UI_Timescale::SetTimeScaleIndex(int timesetting, bool silent) {
	GetTimescaleButtons();
	if (mTimescaleButtons.size() == 4 && mTimescaleButtons[3]) {
		// Timescale2xStop
		mTimescaleButtons[2]->SetEnabled(timesetting == 1);
		mTimescaleButtons[2]->SetVisible(timesetting == 1);
		//Timescale4xStop
		mTimescaleButtons[3]->SetEnabled(timesetting == 2);
		mTimescaleButtons[3]->SetVisible(timesetting == 2);
	}
	// if the game is already at this speed, and no time cheats are applied, return.
	if (GameTimeManager.mSpeedFactors[0] == 1.0f && GameTimeManager.mCurrentSpeedIndex == timesetting) {
		return;
	}
	// Apply FX!
	// make sure this is switched back from any cheats that changed this
	GameTimeManager.SetSpeedFactors(1.0f, 2.0f, 4.0f, 8.0f);
	GameTimeManager.SetSpeed(timesetting);

	if (!silent) {
		PlayTimeSound(timesetting);
	}
	// if silent, force the buttons to unpress
	else if (timesetting == 0 && silent) {
		object_cast<IButton>(mTimescaleButtons[0])->SetButtonStateFlag(kBtnStateSelected, false);
		object_cast<IButton>(mTimescaleButtons[1])->SetButtonStateFlag(kBtnStateSelected, false);
	}
	
}

using namespace Audio;
void UI_Timescale::PlayTimeSound(int timesetting) const {
	switch (timesetting) {
		case 0: PlayAudio(id("timescale_1x")); break;
		case 1: PlayAudio(id("timescale_2x")); break;
		case 2: PlayAudio(id("timescale_4x")); break;
	}
}

//-----------------------------------------------------------------------------

// This method returns a combinations of values in UTFWin::EventFlags.
// The combination determines what types of events (messages) this window procedure receives.
// By default, it receives mouse/keyboard input and advanced messages.
int UI_Timescale::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool UI_Timescale::HandleUIMessage(IWindow* window, const Message& message)
{
	if (message.IsType(kMsgButtonSelect)) {
		auto id = message.source->GetControlID();
		switch (id) {
			case Timescale2x: SetTimeScaleIndex(1); return true;
			case Timescale4x: SetTimeScaleIndex(2); return true;
			case Timescale2xStop: case Timescale4xStop: SetTimeScaleIndex(0); return true;
				
		}
	}
	return false;
}

bool UI_Timescale::HandleMessage(uint32_t messageID, void* msg)
{
	if (!(IsTribeGame() || IsCivGame())) { return false; }

	if (messageID == SimulatorMessages::kMsgSwitchGameMode || messageID == id("ResetTimescale"))
	{
		SetTimeScaleIndex(0,true);
	}
	return false;
}


//-----------------------------------------------------------------------------

// For internal use, do not modify.
int UI_Timescale::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int UI_Timescale::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* UI_Timescale::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(UI_Timescale);
	return nullptr;
}