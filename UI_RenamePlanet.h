#pragma once

#include <Spore\BasicIncludes.h>

#define UI_RenamePlanetPtr intrusive_ptr<UI_RenamePlanet>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;

class UI_RenamePlanet 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("UI_RenamePlanet");
	
	UI_RenamePlanet();
	~UI_RenamePlanet();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	ITextEdit* GetTextEdit() const;
	IWindow* GetTextEditButton() const;
	IWindow* GetTextDisplay() const;

	void BeginEdit();
	void Accept();
	void Close();

	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
private:
	bool mbEditing = false;
};
