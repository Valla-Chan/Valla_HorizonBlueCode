#include "stdafx.h"
#include "EP1_GameplayObjectManager.h"

EP1_GameplayObjectManager::EP1_GameplayObjectManager()
{
	App::AddUpdateFunction(this);
	sInstance = this;

	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, SimulatorMessages::kMsgScenarioRedo);
	MessageManager.AddListener(this, SimulatorMessages::kMsgScenarioUndo);
	MessageManager.AddListener(this, SimulatorMessages::kMsgSwitchGameMode);
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
	if (!mbInEditMode) {
		if (IsEditingAdventure()) {
			mbInEditMode = true;
			PropagateAction(EnterEditMode);
		}
	}
	else {
		if (IsPlayingAdventure()) {
			mbInEditMode = false;
			PropagateAction(EnterPlayMode);
		}
	}
	PropagateAction(DoUpdate);
}

//----------------------------------------------------------------------------------------------------
// Public

EP1_GameplayObject* EP1_GameplayObjectManager::AddGameplayObjectSubmanager(EP1_GameplayObject* object) {
	mObjectSubmanagers.push_back(object);
	return object;
}


//----------------------------------------------------------------------------------------------------
// Private - Object Func Calling

// Make sure to implement all of these funcs into the submanager base class!!
bool EP1_GameplayObjectManager::PropagateAction(Actions action) {
	// loop through all object submanagers
	for (size_t i = 0; i < mObjectSubmanagers.size(); i++) {
		bool value = false;
		auto object = mObjectSubmanagers[i];

		// find what func to perform on all objects
		switch (action) {

			case DoUpdate: object->InternalUpdate(); break;
			case UndoRedo: object->UndoRedo(); break;
			case SwitchToScenario: object->SwitchGameMode(true); break;
			case SwitchAwayFromScenario: object->SwitchGameMode(false); break;
			case UpdateScenarioGoals: object->UpdateScenarioGoals(); break;

			case EnterEditMode: object->EnterMode(EP1_GameplayObject::Mode::EditMode); break;
			case EnterPlayMode: object->EnterMode(EP1_GameplayObject::Mode::PlayMode); break;

			case TakeDamage: object->DoTakeDamage(mpStoredDmgTarget, mStoredDmg, mpStoredDmgAttacker); ClearStoredDmgData(); break;

			// NOTE: these have return cases.
			case UserUIMessage: {
				value = object->UserUIMessage(mStoredUIMessage); ClearStoredMessages();
				if (value) { return true; } break;
			}
			case UserGameMessage: {
				value = object->UserGameMessage(mStoredGameMessage); ClearStoredMessages();
				if (value) { return true; } break;
			}

			case Pickup: {
				value = object->DoPickup();
				if (value) { return true; } break;
			}
			case Drop: {
				value = object->DoDrop();
				if (value) { return true; } break;
			}	
			case Moved: {
				value = object->DoMoved();
				if (value) { return true; } break;
			}
			


		}
	}
	
	return false;

}


//----------------------------------------------------------------------------------------------------
// Private - Input Management

// NOTE: this func does not detect if an object is selected!
// Consider making the propagation call a private version of the object->func(), that only calls the user-defined if the selection criteria is met.
bool EP1_GameplayObjectManager::HandleMouseInput(const Message& message) {
	if (IsEditingAdventure()) {

		// Player has released mouse
		// TODO: check if they released the left mouse... normal methods dont work.
		if (mbMouseDown && message.IsType(kMsgMouseUp)) {
			mbMouseDown = false;
			return PropagateAction(Drop);
		}
		else if (message.Mouse.IsLeftButton()) {
			
			// Player has moved the mouse
			if (mbMouseDown && message.IsType(kMsgMouseMove)) {
				return PropagateAction(Moved);
			}
			// Player has left clicked the mouse
			else if (!mbMouseDown && message.IsType(kMsgMouseDown)) {
				mbMouseDown = true;
				return PropagateAction(Pickup);
			}
		}
		
	}
	else if (IsPlayingAdventure()) {
		// TODO: add click / select funcs into this, for stuff like driving cars
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
		auto task = Simulator::ScheduleTask(this, &EP1_GameplayObjectManager::FireUndoRedo, 0.01f);
	}
	else if (messageID == kMsgSwitchGameMode) {
		auto task = Simulator::ScheduleTask(this, &EP1_GameplayObjectManager::FireSwitchIfScenario, 0.2f);

	}
	return false;
}

//---------------------------------------
// fire these from a delay

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

void EP1_GameplayObjectManager::FireUndoRedo() {
	if (IsScenarioMode()) { PropagateAction(UndoRedo); }
}
//---------------------------------------

void EP1_GameplayObjectManager::ClearStoredMessages() {
	mStoredUIMessage = Message();
	mStoredGameMessage = 0x0;
}

void EP1_GameplayObjectManager::ClearStoredDmgData() {
	mStoredDmg = 0.0f;
	mpStoredDmgTarget = nullptr;
	mpStoredDmgAttacker = nullptr;
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
