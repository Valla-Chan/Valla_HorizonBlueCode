#include "stdafx.h"
#include "MouseClickRegister.h"

MouseClickRegister::MouseClickRegister()
{
	sInstance = this;
	WindowManager.GetMainWindow()->AddWinProc(this);
}


MouseClickRegister::~MouseClickRegister()
{
	sInstance = nullptr;
}

MouseClickRegister* MouseClickRegister::sInstance;

MouseClickRegister* MouseClickRegister::Get()
{
	return sInstance;
}

// For internal use, do not modify.
int MouseClickRegister::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int MouseClickRegister::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* MouseClickRegister::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(MouseClickRegister);
	return nullptr;
}
//---------------------------------------------------------------------------------

void MouseClickRegister::ClickedObject(cGameData* object, MouseButton button) {
	ClickMessage* message = new(ClickMessage);
	message->object = object;
	message->mouseButton = button;
	MessageManager.MessageSend(CLICKMESSAGE_ID, message);
}

//---------------------------------------------------------------------------------

int MouseClickRegister::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

bool MouseClickRegister::HandleUIMessage(IWindow* window, const Message& message)
{
	auto hoveredObject = GameViewManager.GetHoveredObject();
	if (hoveredObject) {

		// Clicked some button.
		// Check if interruption or new mouse click.
		if (message.IsType(kMsgMouseDown)) {
			// New mouse click, store data
			if (mMouseButton == -1 && !mbMouseInterrupted) {
				mMouseButton = message.Mouse.mouseButton;
				mbMouseInterrupted = false;
				mpStoredObject = GameViewManager.GetHoveredObject();
				//mpStoredWindow = ;
			}
			// Click is an interruption to another click, invalidate.
			else {
				mbMouseInterrupted = true;
			}
		}

		// Unclicked some button.
		// Check the data of this message to see if we should end the click.
		else if (message.IsType(kMsgMouseUp)) {

			// If the button unclicked matches the stored button value,
			// clear the stored data.
			if (message.Mouse.mouseButton == mMouseButton) {
				mpStoredObject = nullptr;
				//mpStoredWindow = nullptr;
			}

			// If interrupted, do nothing.
			if (mbMouseInterrupted) { return false; }

		}

		// Moving mouse always counts as an interruption.
		else if (message.IsType(kMsgMouseMove)) {
			mbMouseInterrupted = true;
		}

	}
	

	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}
