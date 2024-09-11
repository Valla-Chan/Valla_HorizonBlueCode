#pragma once

#include <Spore\BasicIncludes.h>

#define EP1_GameplayObjectPtr intrusive_ptr<EP1_GameplayObject>

using namespace UTFWin;
using namespace Simulator;

class EP1_GameplayObject 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObject");
	
	EP1_GameplayObject();
	~EP1_GameplayObject();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;

	//-------------------------------------------

	cSpatialObjectPtr pSelectedObject = nullptr;

	// States
	static bool IsPlayingAdventure();
	static bool IsEditingAdventure();

	// Get Object Pointers
	static cCreatureAnimalPtr GetClosestCreature(cSpatialObjectPtr gameplayob, float max_distance, bool exclude_avatar = false);
	static cSpatialObjectPtr GetRolledObject();
	static cCreatureAnimalPtr GetRolledCreature();

	// Inputs
	virtual void Pickup() = 0;
	virtual void Drop() = 0;
	virtual void Moved() = 0;

	// Default Message Behaviors
	bool HandleMouseInput(const Message& message) {
		// released mouse
		// TODO: detect if this is the left mouse. Usual methods to check this do not work.
		if (message.IsType(kMsgMouseUp) && pSelectedObject) {
			Drop();
			return true;
		}

		// Player has left clicked the mouse
		else if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseDown) && !pSelectedObject) {
			Pickup();
			return true;
		}

		// Player has moved the mouse
		else if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseMove)) {
			// already held, therefor move it
			if (pSelectedObject) {
				Moved();
				return true;
			}

		}
		return false;
	}

	// Virtuals - Game Messages
	virtual void UserMessage(uint32_t messageID) = 0;
	virtual void UndoRedo() = 0;
	void SwitchMode() { auto task = Simulator::ScheduleTask(this, &EP1_GameplayObject::FireIfScenario, 0.2f); }


	//-------------------------------------------
	// Message Funcs

	int GetEventFlags() const override { return kEventFlagBasicInput; }
	virtual void FireIfScenario() = 0;

	bool HandleUIMessage(IWindow* pWindow, const Message& message) override {
		// cancel if not in edit mode.
		if (!(IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::EditMode)) { return false; }
		bool mouseInput = this->HandleMouseInput(message);
		return false;
	}
	
	bool HandleMessage(uint32_t messageID, void* msg) override {
		if (messageID == kMsgScenarioRedo || messageID == kMsgScenarioUndo) {
			if (IsScenarioMode()) { this->UndoRedo(); }
		}
		else if (messageID == kMsgSwitchGameMode) {
			this->SwitchMode();
		}
		return false;
	}
	
};
