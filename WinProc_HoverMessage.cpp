#include "stdafx.h"
#include "WinProc_HoverMessage.h"

WinProc_HoverMessage::WinProc_HoverMessage(uint32_t hovermsg, uint32_t unhovermsg)
{
	mHoverMessage = hovermsg;
	mUnHoverMessage = unhovermsg;
}


WinProc_HoverMessage::~WinProc_HoverMessage()
{
}

// For internal use, do not modify.
int WinProc_HoverMessage::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int WinProc_HoverMessage::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* WinProc_HoverMessage::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(WinProc_HoverMessage);
	return nullptr;
}

int WinProc_HoverMessage::GetEventFlags() const {
	return UTFWin::kEventRefresh;
}

bool WinProc_HoverMessage::HandleUIMessage(IWindow* pWindow, const Message& message) {
	if (message.IsType(kMsgMouseEnter)) {
		if (mHoverMessage != 0x0) {
			MessageManager.MessageSend(mHoverMessage, pWindow);
		}
	}
	else if (message.IsType(kMsgMouseLeave)) {
		if (mUnHoverMessage != 0x0) {
			MessageManager.MessageSend(mUnHoverMessage, pWindow);
		}
	}
	return false;
}