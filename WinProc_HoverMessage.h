#pragma once

#include <Spore\BasicIncludes.h>

#define WinProc_HoverMessagePtr intrusive_ptr<WinProc_HoverMessage>

using namespace UTFWin;

class WinProc_HoverMessage 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("WinProc_HoverMessage");
	
	WinProc_HoverMessage(uint32_t hovermsg = 0x0, uint32_t unhovermsg = 0x0);
	~WinProc_HoverMessage();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	uint32_t mHoverMessage = 0x0;
	uint32_t mUnHoverMessage = 0x0;

	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
};
