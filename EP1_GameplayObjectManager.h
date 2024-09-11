#pragma once

#include <Spore\BasicIncludes.h>
#include "EP1_GameplayObject.h"

#define EP1_GameplayObjectManagerPtr intrusive_ptr<EP1_GameplayObjectManager>

using namespace UTFWin;
using namespace Simulator;

class EP1_GameplayObjectManager 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObjectManager");
	
	EP1_GameplayObjectManager();
	~EP1_GameplayObjectManager();
	static EP1_GameplayObjectManager* Get();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;

	//---------------------------------------

	enum Actions {
		DoUpdate,
		UndoRedo,
		SwitchToScenario,
		SwitchAwayFromScenario,

		UserUIMessage,
		UserGameMessage,

		Pickup,
		Drop,
		Moved,

	};

	EP1_GameplayObjectPtr AddGameplayObjectSubmanager(EP1_GameplayObjectPtr object);

//----------------------------------------------------------------------------------------
private:
	bool mbInScenarioMode = false;

	static bool IsPlayingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode); }
	static bool IsEditingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::EditMode); }


//-------------------------------------------------
// Object Management
	
	// List of all classes that define gameplay object behaviors
	eastl::vector<EP1_GameplayObjectPtr> mObjectSubmanagers = {};

	// send an action to all stored object submanagers
	bool PropagateAction(Actions action);

//-------------------------------------------------
// Input Management

	bool HandleMouseInput(const Message& message);

//-------------------------------------------------
// Message Handling

	int GetEventFlags() const override;
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;
	void FireSwitchIfScenario();

protected:
	void ClearStoredMessages();
	Message& mStoredUIMessage = Message();
	uint32_t mStoredGameMessage = 0x0;

	static EP1_GameplayObjectManager* sInstance;
};
