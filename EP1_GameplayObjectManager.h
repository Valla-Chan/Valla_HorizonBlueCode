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
		UpdateScenarioGoals,
		EnterEditMode,
		EnterPlayMode,

		TakeDamage,

		UserUIMessage,
		UserGameMessage,
		
		//Editor
		Pickup,
		Drop,
		Moved,

		// Ingame
		MouseClick,
		MouseRelease,
		MouseDrag,
	};

	EP1_GameplayObject* AddGameplayObjectSubmanager(EP1_GameplayObject* object);

	// send an action to all stored object submanagers
	bool PropagateAction(Actions action);

	// handle damage data, then propagate it to submanagers
	void DoTakeDamage(cCombatantPtr target, float damage, cCombatantPtr attacker) {
		mpStoredDmgTarget = target;
		mpStoredDmgAttacker = attacker;
		mStoredDmg = damage;
		PropagateAction(TakeDamage);
	}

//----------------------------------------------------------------------------------------
private:
	bool mbInScenarioMode = false;
	bool mbInEditMode = false;

	static bool IsPlayingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode); }
	static bool IsEditingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::EditMode); }


//-------------------------------------------------
// Object Management
	
	// List of all classes that define gameplay object behaviors
	eastl::vector<EP1_GameplayObjectPtr> mObjectSubmanagers = {};

//-------------------------------------------------
// Input Management

	bool HandleMouseInput(const Message& message);

//-------------------------------------------------
// Message Handling

	int GetEventFlags() const override;
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;
	
	// fire these from a delay
	void FireSwitchIfScenario();
	void FireUndoRedo();

protected:
	bool mbMouseDown = false;

	// Stored Message Data
	void ClearStoredMessages();
	Message& mStoredUIMessage = Message();
	uint32_t mStoredGameMessage = 0x0;

	// Stored Damage Data
	void ClearStoredDmgData();
	float mStoredDmg = 0.0f;
	cCombatantPtr mpStoredDmgTarget;
	cCombatantPtr mpStoredDmgAttacker;

	static EP1_GameplayObjectManager* sInstance;
};
