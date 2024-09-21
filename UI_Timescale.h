#pragma once

#include <Spore\BasicIncludes.h>

#define UI_TimescalePtr intrusive_ptr<UI_Timescale>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;

class UI_Timescale 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("UI_Timescale");
	
	UI_Timescale();
	~UI_Timescale();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	//----------------------------------------

	enum TimeButtons {
		Pause = 0xD305CA84,
		Timescale2x	=		0xE6607B05,
		Timescale2xStop	=	0xE6607B06,
		Timescale4x	=		0xE6607B07,
		Timescale4xStop	=	0xE6607B08,
	};
	// tribe or civ timescale buttons
	eastl::vector<IWindow*> mTimescaleButtons = {};

	eastl::vector<IWindow*> GetTimescaleButtons();

	void SetTimeScaleIndex(int timesetting, bool silent = false);
	void PlayTimeSound(int timesetting) const;
	


	//----------------------------------------
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;
	
};
