#include "stdafx.h"
#include "UI_RenamePlanet.h"

UI_RenamePlanet::UI_RenamePlanet()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
}


UI_RenamePlanet::~UI_RenamePlanet()
{
}

// For internal use, do not modify.
int UI_RenamePlanet::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int UI_RenamePlanet::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* UI_RenamePlanet::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(UI_RenamePlanet);
	return nullptr;
}

//------------------------------------------------------------------------------------------

ITextEdit* UI_RenamePlanet::GetTextEdit() const {
	auto window = WindowManager.GetMainWindow();
	return object_cast<ITextEdit>(window->FindWindowByID(0x0562A2D9)); // find the planet rename text edit

}

IWindow* UI_RenamePlanet::GetTextEditButton() const {
	auto window = WindowManager.GetMainWindow();
	return window->FindWindowByID(0x0562A2D8); // find the planet rename text edit button (activates the text edit)
}

IWindow* UI_RenamePlanet::GetTextDisplay() const {
	auto window = WindowManager.GetMainWindow();
	return window->FindWindowByID(0x0562A2DA); // find the planet name display
}

//------------------------------------------------

void UI_RenamePlanet::BeginEdit() {
	GameNounManager.GetAvatar()->func74h(GameNounManager.GetAvatar());

	// make sure we have a planet to edit
	auto planetRecord = Simulator::GetActivePlanetRecord();
	if (!planetRecord) { return; }

	auto textEdit = GetTextEdit();
	auto textDisplay = GetTextDisplay();

	if (textEdit && textDisplay) {
		// set the text edit text to the planet's current name
		auto nameString = planetRecord->mName;
		textEdit->ToWindow()->SetCaption(nameString.c_str());

		// change visibility of windows to show the text edit
		textDisplay->SetVisible(false);
		textEdit->ToWindow()->SetVisible(true);
		// focus the text edit


		mbEditing = true;
	}
}

void UI_RenamePlanet::Accept() {
	auto textEdit = GetTextEdit();
	auto textDisplay = GetTextDisplay();

	if (textEdit && textDisplay) {
		auto nameString = string16(textEdit->ToWindow()->GetCaption());

		// make sure the name is long enough, and if not do not set.
		if (nameString.length() > 0) {
			auto planetRecord = Simulator::GetActivePlanetRecord();
			planetRecord->mName = nameString;
			textDisplay->SetCaption(nameString.c_str());
		}
		// change visibility of windows back to normal
		textDisplay->SetVisible(true);
		textEdit->ToWindow()->SetVisible(false);	
	}
	mbEditing = false;
}

void UI_RenamePlanet::Close() {
	auto textEdit = GetTextEdit();
	auto textDisplay = GetTextDisplay();

	if (textEdit && textDisplay) {
		// change visibility of windows back to normal
		textDisplay->SetVisible(true);
		textEdit->ToWindow()->SetVisible(false);
	}
	mbEditing = false;
}



//------------------------------------------------------------------------------------------

// This method returns a combinations of values in UTFWin::EventFlags.
// The combination determines what types of events (messages) this window procedure receives.
// By default, it receives mouse/keyboard input and advanced messages.
int UI_RenamePlanet::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool UI_RenamePlanet::HandleUIMessage(IWindow* window, const Message& message)
{
	// only allow in these stages
	if (!(Simulator::IsCreatureGame() || Simulator::IsTribeGame() || Simulator::IsCivGame() || Simulator::IsSpaceGame())) { return false; }
	if (Simulator::IsLoadingGameMode()) { return false; }

	// pressed enter or escape or etc
	if (message.IsType(kMsgKeyDown) && mbEditing) {
		// accept
		if (message.Key.vkey == VK_CLEAR || message.Key.vkey == VK_RETURN) {
			Accept();
			return true;
		}
		// cancel
		else if (message.Key.vkey == VK_ESCAPE) {
			Close();
			return true;
		}
	}

	//------------------------------------------------------
	// Rename Planet UI Button

	else if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();

		// not yet editing the name
		if (!mbEditing) {
			// if clicked the Rename Button
			if (id == 0x0562A2D8) {
				BeginEdit();
			}
		}
		// currently editing the name
		else {
			// if clicked something other than the Text Edit
			if (id != 0x0562A2D9) {
				Accept();
			}
		}
		
	}
	// clicked mouse, check if we should stop editing
	else if (message.IsType(kMsgMouseDown)) {
		if (mbEditing) {
			Accept();
		}
	}


	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}
