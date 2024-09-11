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

eastl::vector<cSpatialObjectPtr> EP1_GameplayObject::GetAllObjects() {
	// loop thru all objects
	auto objects = Simulator::GetDataByCast<Simulator::cSpatialObjectPtr>();
	for (auto object : objects) {
		if (IsHa)
	}
}

// Get the closest gameplay object to a creature
// TODO: this! make sure to check if the distance is both within last_dist and within the object's max radius, after finding an object.
cSpatialObjectPtr EP1_GameplayObject::GetClosestObject(cCreatureAnimalPtr creature, bool exclude_avatar) {
	auto origin = creature->GetPosition();
	float last_dist = 4096;

	cSpatialObjectPtr found_object = nullptr;
	if (creature == GameNounManager.GetAvatar() && exclude_avatar) {
		return nullptr;
	}

	// loop thru all objects
	auto objects = Simulator::GetDataByCast<Simulator::cSpatialObjectPtr>();
	for (auto object : objects) {

	}

}

// Get the closest creature to a gameplay object
cCreatureAnimalPtr EP1_GameplayObject::GetClosestCreature(cSpatialObjectPtr object, bool exclude_avatar) {
	auto origin = gameplayob->GetPosition();
	float last_dist = GetObjectMaxRadius(object);

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
// Virtuals - Object Definition

// Return true if an object is one that is handled by this class.
bool EP1_GameplayObject::IsHandledObject(cSpatialObjectPtr object) {
	return false;
}

float EP1_GameplayObject::GetObjectMaxRadius(cSpatialObjectPtr object) {
	return 4.0f * object->GetScale();
}

//-----------------------------------------------------------------------------------------------
// Virtuals - Actions from Manager

void EP1_GameplayObject::ApplyAllObjectEffects() {
	for (auto object : GetAllObjects()) {
		ApplyObjectEffect(object);
	}
}

// pure virtual, must be overriden
/*
void EP1_GameplayObject::ApplyObjectEffect(cSpatialObjectPtr object) {
}

void EP1_GameplayObject::ApplyCreatureEffect(cCreatureAnimalPtr creature, cSpatialObjectPtr object) {
}
*/

void EP1_GameplayObject::UndoRedo() {
	ApplyAllObjectEffects();
}

void EP1_GameplayObject::SwitchMode(bool to_scenario) {
	if (to_scenario) {
		ApplyAllObjectEffects();
	}
	
}





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
