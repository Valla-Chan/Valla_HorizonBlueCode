#include "stdafx.h"
#include "LOAD_LoadingSpeed.h"

LOAD_LoadingSpeed::LOAD_LoadingSpeed()
{
	App::ConsolePrintF("Initializing Message Handler ");
}


LOAD_LoadingSpeed::~LOAD_LoadingSpeed()
{
}


bool LOAD_LoadingSpeed::HandleMessage(uint32_t messageID, void* msg)
{
	App::ConsolePrintF("Handle Message?");
	if (messageID == Simulator::kMsgSwitchGameMode)
	{
		
		App::ConsolePrintF("Message handled.");
		if (Simulator::IsLoadingGameMode())
		{
			App::ConsolePrintF("Speed up.");
			GameTimeManager.SetSpeed(3);
		}
		else {
			App::ConsolePrintF("Slow back down");
			GameTimeManager.SetSpeed(0);
		}
	}
		
	return false;
}

// For internal use, do not modify.
int LOAD_LoadingSpeed::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int LOAD_LoadingSpeed::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* LOAD_LoadingSpeed::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(LOAD_LoadingSpeed);
	return nullptr;
}
