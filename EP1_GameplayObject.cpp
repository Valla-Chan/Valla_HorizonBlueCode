#include "stdafx.h"
#include "EP1_GameplayObject.h"

EP1_GameplayObject::EP1_GameplayObject()
{
	App::AddUpdateFunction(this);
}


EP1_GameplayObject::~EP1_GameplayObject()
{
}

void EP1_GameplayObject::Update() {

}

//-----------------------------------------------------------------------------------------------

bool EP1_GameplayObject::IsPlayingAdventure() {
	return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode);
}
bool EP1_GameplayObject::IsEditingAdventure() {
	return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::EditMode);
}

//-----------------------------------------------------------------------------------------------

// Get the closest creature to a gameplay object
cCreatureAnimalPtr EP1_GameplayObject::GetClosestCreature(cSpatialObjectPtr gameplayob, float max_distance, bool exclude_avatar) {
	auto origin = gameplayob->GetPosition();
	float last_dist = max_distance; //gameplayob->GetScale() * 4.0f;

	cCreatureAnimalPtr found_creature = nullptr;
	auto avatar = GameNounManager.GetAvatar();

	// loop thru all creatures
	auto creatures = Simulator::GetDataByCast<Simulator::cCreatureAnimal>();
	for (auto creature : creatures) {
		float dist = Math::distance(origin, creature->GetPosition());

		if (dist < last_dist && (!exclude_avatar || creature != avatar)) {
			found_creature = creature;
			last_dist = dist;
		}
	}
	return found_creature;
}


cSpatialObjectPtr EP1_GameplayObject::GetRolledObject() {
	auto sus = Simulator::cSpatialObject;
	auto objects = Simulator::GetDataByCast<Simulator::cSpatialObject>();
	for (auto object : objects) {
		if (object->IsRolledOver()) {
			return object;
		}
	}
	return nullptr;
}

cCreatureAnimalPtr EP1_GameplayObject::GetRolledCreature() {
	auto objects = Simulator::GetDataByCast<Simulator::cCreatureAnimal>();
	for (auto object : objects) {
		if (object->IsRolledOver()) {
			return object;
		}
	}
	return nullptr;
}


//-----------------------------------------------------------------------------------------------

// UI Window Messages

/*
bool EP1_GameplayObject::HandleMouseInput(const Message& message) {
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
*/

//-----------------------------------------------------------------------------------------------

/*
void EP1_GameplayObject::FireIfScenario() {
	if (IsScenarioMode()) {
		return;
	}
	return;
}*/

//-----------------------------------------------------------------------------------------------


// For internal use, do not modify.
int EP1_GameplayObject::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EP1_GameplayObject::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_GameplayObject::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(EP1_GameplayObject);
	return nullptr;
}
