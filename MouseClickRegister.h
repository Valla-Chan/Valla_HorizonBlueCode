#pragma once

#include <Spore\BasicIncludes.h>

#define MouseClickRegisterPtr intrusive_ptr<MouseClickRegister>

// To avoid repeating UTFWin:: all the time.
using namespace Simulator;
using namespace UTFWin;

class MouseClickRegister 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("MouseClickRegister");
	
	MouseClickRegister();
	~MouseClickRegister();
	static MouseClickRegister* Get();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	//-----------------------------------------
	// Sent when an object is clicked
	const uint32_t CLICKMESSAGE_ID = id("ClickedObject");
	//bool left_pressed = false;
	//bool right_pressed = false;

	struct ClickMessage
	{
		cGameData* object;
		int mouseButton;
	};

	/// Player has clicked and released the mouse on one Simulator Object without interruption
	/// Emits a message.
	void ClickedObject(cGameData* object, MouseButton button);

	/// Player has clicked and released the mouse on one SPUI Window without interruption
	//void ClickedWindow(IWindow* window, MouseButton button);

	//-----------------------------------------
	int GetEventFlags() const override;
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

private:
	int mMouseButton = -1; // what mouse button are we tracking?
	//bool mbMouseMoved = false; // has the mouse moved since clicking? 
	bool mbMouseInterrupted = false; // has an interruption occured since clicking?

	cGameData* mpStoredObject = nullptr; // object to check if still hovering
	//IWindow* mpStoredWindow = nullptr; // object to check if still hovering

protected:
	static MouseClickRegister* sInstance;
};
