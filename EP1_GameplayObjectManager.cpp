#include "stdafx.h"
#include "EP1_GameplayObjectManager.h"

EP1_GameplayObjectManager::EP1_GameplayObjectManager()
{
	App::AddUpdateFunction(this);
	sInstance = this;
}

EP1_GameplayObjectManager::~EP1_GameplayObjectManager()
{
	sInstance = nullptr;
}

EP1_GameplayObjectManager* EP1_GameplayObjectManager::sInstance;

EP1_GameplayObjectManager* EP1_GameplayObjectManager::Get()
{
	return sInstance;
}

void EP1_GameplayObjectManager::Update()
{
	PropagateAction(DoUpdate);
}

//----------------------------------------------------------------------------------------------------
// Public

EP1_GameplayObjectPtr EP1_GameplayObjectManager::AddGameplayObjectSubmanager(EP1_GameplayObjectPtr object) {
	mObjectSubmanagers.push_back(object);
	return object;
}


//----------------------------------------------------------------------------------------------------
// Private - Object Func Calling

// Make sure to implement all of these funcs into the submanager base class!!
bool EP1_GameplayObjectManager::PropagateAction(Actions action) {
	// loop through all object submanagers
	for (size_t i = 0; i < mObjectSubmanagers.size(); i++) {
		auto object = mObjectSubmanagers[i];

		// find what func to perform on all objects
		switch (action) {

		case DoUpdate: object->Update(); break;
		case UndoRedo: object->UndoRedo(); break;
		case SwitchToScenario: object->SwitchMode(true); break;
		case SwitchAwayFromScenario: object->SwitchMode(false); break;

		// NOTE: these have return cases.
		case UserUIMessage:		bool value = object->UserUIMessage(mStoredUIMessage);	  ClearStoredMessages();			if (value) { return true; } break;
		case UserGameMessage:	bool value = object->UserGameMessage(mStoredGameMessage);  ClearStoredMessages();	if (value) { return true; } break;

		case Pickup: bool value = object->Pickup();		if (value) { return true; } break;
		case Drop: bool value = object->Drop();			if (value) { return true; } break;
		case Moved: bool value = object->Moved();		if (value) { return true; } break;


		}
	}
	
	return false;

}


//----------------------------------------------------------------------------------------------------
// Private - Input Management

// NOTE: this func does not detect if an object is selected!
// Consider making the propagation call a private version of the object->func(), that only calls the user-defined if the selection criteria is met.
bool EP1_GameplayObjectManager::HandleMouseInput(const Message& message) {
	PropagateAction(Drop);

	if (IsEditingAdventure()) {
		// Player has released mouse
		// TODO: detect if this is the left mouse. Usual methods to check this do not work.
		if (message.IsType(kMsgMouseUp)) { // && pSelectedObject
			return PropagateAction(Drop);
		}

		// Player has left clicked the mouse
		else if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseDown)) { // && !pSelectedObject
			return PropagateAction(Pickup);
		}

		// Player has moved the mouse
		else if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseMove)) {
			return PropagateAction(Moved);

		}
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------
// Private - Message Management

int EP1_GameplayObjectManager::GetEventFlags() const
{
	return kEventFlagBasicInput;
}


bool EP1_GameplayObjectManager::HandleUIMessage(IWindow* window, const Message& message)
{
	mStoredUIMessage = message;
	return HandleMouseInput(message);
}

bool EP1_GameplayObjectManager::HandleMessage(uint32_t messageID, void* msg) {
	mStoredGameMessage = messageID;
	
	if (messageID == kMsgScenarioRedo || messageID == kMsgScenarioUndo) {
		if (IsScenarioMode()) { PropagateAction(UndoRedo); }
	}
	else if (messageID == kMsgSwitchGameMode) {
		auto task = Simulator::ScheduleTask(this, &EP1_GameplayObjectManager::FireSwitchIfScenario, 0.2f);

	}
	return false;
}

// fire switchmode, only if in scenario.
void EP1_GameplayObjectManager::FireSwitchIfScenario() {
	if (IsScenarioMode()) {
		PropagateAction(SwitchToScenario);
		mbInScenarioMode = true;
	}
	else if (mbInScenarioMode) {
		PropagateAction(SwitchAwayFromScenario);
		mbInScenarioMode = false;
	}
}

void EP1_GameplayObjectManager::ClearStoredMessages() {
	mStoredUIMessage = Message();
	mStoredGameMessage = 0x0;
}

//----------------------------------------------------------------------------------------------------

// For internal use, do not modify.
int EP1_GameplayObjectManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EP1_GameplayObjectManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_GameplayObjectManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(EP1_GameplayObjectManager);
	return nullptr;
}
