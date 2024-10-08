#include "stdafx.h"
#include "CRG_GameplayAbilities.h"
#include "AddressCheat.h"

CRG_GameplayAbilities::CRG_GameplayAbilities()
{
	//MessageManager.AddListener(this, id("BabyGrowUp"));
	WindowManager.GetMainWindow()->AddWinProc(this);
}


CRG_GameplayAbilities::~CRG_GameplayAbilities()
{
}

// For internal use, do not modify.
int CRG_GameplayAbilities::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CRG_GameplayAbilities::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* CRG_GameplayAbilities::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(CRG_GameplayAbilities);
	return nullptr;
}

// This method returns a combinations of values in UTFWin::EventFlags.
// The combination determines what types of events (messages) this window procedure receives.
// By default, it receives mouse/keyboard input and advanced messages.
int CRG_GameplayAbilities::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool CRG_GameplayAbilities::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!Simulator::IsCreatureGame()) { return false; }

	//------------------------------------------------------
	// Inventory UI Buttons

	if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();
		// Sleep
		if (id == 0x84AE4DA0) {
			CinematicManager.PlayCinematic("CRG_AvatarSleep", 0, 0, 0, 0, 0);
		}
		// Build Nest
		else if (id == 0x84AE4DA1) {
			//CinematicManager.PlayCinematic("CRG_BuildNest", 0, 0, 0, 0, 0);
		}
	}


	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}
