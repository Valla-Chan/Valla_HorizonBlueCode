#include "stdafx.h"
#include "CRG_ObjectManager.h"
#include <Spore\Simulator.h>
#include "CapabilityChecker.h"


cInteractiveOrnament* cObjectManager::GetSelectedObject() const {
	// loop through all ingame interactables
	auto interactables = GetData<cInteractiveOrnament>();
	for (auto object : interactables) {
		App::ConsolePrintF("Selected: %x", object->IsSelected());

		if (object->IsSelected()) {
			return object.get();
		}
	}
	return nullptr;
}

bool cObjectManager::AvatarIsCarnivore() const {
	cCreatureAnimal* avatar = GameNounManager.GetAvatar();
	if (avatar) {
		return CapabilityChecker.GetCapabilityLevel(avatar, 0x073CE5DD) > 0;
	}
	else { return false; }
}

void cObjectManager::StartWaitingForNoun() {
	waiting_for_noun = true;
}

// This is called when and interactable object is successfully destroyed.
bool cObjectManager::HandleMessage(uint32_t messageID, void* msg)
{
	App::ConsolePrintF("Handle Message?");
	if (messageID == Simulator::kMsgGameNounStatusChanged) {
		App::ConsolePrintF("Noun Status Changed");
		// use casting to get an Anim::AnimationMessage from the msg variable
		//auto newMsg = (Anim::AnimationMessage*)msg;
		
		//newMsg->
	}

	return false;
}




cObjectManager::cObjectManager()
{
}


cObjectManager::~cObjectManager()
{
}

cObjectManager* cObjectManager::sInstance;

cObjectManager* cObjectManager::Get()
{
	return sInstance;
}

// For internal use, do not modify.
int cObjectManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cObjectManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cObjectManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(cObjectManager);
	return nullptr;
}
