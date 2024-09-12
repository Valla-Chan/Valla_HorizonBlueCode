#include "stdafx.h"
#include "EP1_GameplayObject.h"

EP1_GameplayObject::EP1_GameplayObject()
{
}


EP1_GameplayObject::~EP1_GameplayObject()
{
}

//-----------------------------------------------------------------------------------------------

eastl::vector<cSpatialObjectPtr> EP1_GameplayObject::GetAllObjects() {
	eastl::vector<cSpatialObjectPtr> found_objects = {};
	// loop thru all objects
	auto objects = GetDataByCast<cSpatialObject>();
	for (auto object : objects) {
		if (IsHandledObject(object)) {
			found_objects.push_back(object);
		}
	}
	return found_objects;
}

void EP1_GameplayObject::StoreObjects() {
	// Slightly faster to repeat the code from GetAllObjects() than looping twice.
	mpIngameObjects.clear();
	// loop thru all objects
	auto objects = GetDataByCast<cSpatialObject>();
	for (auto object : objects) {
		if (IsHandledObject(object)) {
			mpIngameObjects.push_back(GameplayObject(object, GetClosestCombatant(object)));
		}
	}
}


tGameDataVectorT<cCombatant> EP1_GameplayObject::GetCombatantData() const {
	//if (mbCombatantCanBeCreature && (!mbCombatantCanBeVehicle && !mbCombatantCanBeBuilding && !mbCombatantCanBeMisc)) {
	//	return Simulator::GetDataByCast<cCreatureAnimal>();
	//}
	return Simulator::GetDataByCast<cCombatant>();
}

// Get the closest handled gameplay object to a source object (often a combatant but can be anything)
// TODO: this! make sure to check if the distance is both within last_dist and within the object's max radius, after finding an object.
cSpatialObjectPtr EP1_GameplayObject::GetClosestHandledObject(cSpatialObjectPtr srcobject) {
	auto origin = srcobject->GetPosition();
	float last_dist = 4096;

	{ // do not allow invalid combatants to check for their nearest object!
		auto combatant = object_cast<cCombatant>(srcobject);
		if (!IsValidCombatantType(combatant)) { return nullptr; }
	}

	cSpatialObjectPtr found_object = nullptr;
	// if set to exclude avatar, do not find the object nearest to them.
	if (srcobject == object_cast<cSpatialObject>(GameNounManager.GetAvatar()) && mbExcludeAvatar) {
		return nullptr;
	}
	
	// loop thru all objects
	auto objects = GetAllObjects();
	for (auto object : objects) {
		// do not allow selecting the same object as the src
		if (srcobject != object) {
			float dist = Math::distance(origin, object->GetPosition());
			if (dist < last_dist && dist < GetObjectMaxRadius(object) && (!mbExcludeAvatar || object != object_cast<cSpatialObject>(Avatar()))) {
				found_object = object;
				last_dist = dist;
			}
		}
	}
	return found_object;
}

// Get the closest combatant to a gameplay object.
// Get the closest combatant to a gameplay object.
cCombatantPtr EP1_GameplayObject::GetClosestCombatant(cSpatialObjectPtr srcobject) {
	auto origin = srcobject->GetPosition();
	float last_dist = GetObjectMaxRadius(srcobject);

	cCombatantPtr found_combatant = nullptr;
	auto avatar = GameNounManager.GetAvatar();

	// loop thru all creatures
	auto combatants = GetCombatantData();
	for (auto combatant : combatants) {
		auto spatial = object_cast<cSpatialObject>(combatant);
		if (spatial) {
			float dist = Math::distance(origin, GetObjectPos(spatial));
			float dist2 = Math::distance(origin, spatial->GetPosition()); // backup dist check

			if ((!mbExcludeAvatar || combatant != object_cast<cCombatant>(avatar)) && IsValidCombatantType(combatant)) {
				if (dist < last_dist) {
					found_combatant = combatant;
					last_dist = dist;
				}
				// backup dist check
				else if (dist2 < last_dist) {
					found_combatant = combatant;
					last_dist = dist2;
				}
			}

		}
	}
	return found_combatant;
}


cSpatialObjectPtr EP1_GameplayObject::GetRolledHandledObject() {
	auto objects = GetDataByCast<cSpatialObject>();
	for (auto object : objects) {
		if (object->IsRolledOver()) {
			return object;
		}
	}
	return nullptr;
}

cCombatantPtr EP1_GameplayObject::GetRolledCombatant() {
	auto objects = GetCombatantData();
	for (auto object : objects) {
		auto spatial = object_cast<cSpatialObject>(object);
		if (spatial && spatial->IsRolledOver()) {
			return object;
		}
	}
	return nullptr;
}

EP1_GameplayObject::GameplayObject EP1_GameplayObject::GetIngameObject(cSpatialObjectPtr srcobject) {
	for (auto item : mpIngameObjects) {
		if (item.object == srcobject) {
			return item;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Non-Virtual Helper Funcs

void EP1_GameplayObject::InternalUpdate() {
	Update();
	CheckRadius();
}

//check all objects to see if anything is in range or out of range of them
void EP1_GameplayObject::CheckRadius() {
	if (mbCheckRadiusPerFrame) {
		for (auto item : mpIngameObjects) {
			auto activator = GetClosestCombatant(item.object);
			// item had an activator, check if it still does
			if (item.activator && !activator) {
				item.activator = nullptr;
				OnExitRadius(item.object, item.activator);
			}
			// item had NO activator, check if it does now
			else if (!item.activator && activator) {
				item.activator = activator;
				OnEnterRadius(item.object, item.activator);
			}
		}

	}
}

// Input Precursors

bool EP1_GameplayObject::DoPickup() {
	bool value = false;
	// only pickup if not already holding
	if (!pHeldObject) {
		pHeldObject = object_cast<cSpatialObject>(GetRolledCombatant());
		if (pHeldObject) {
			value = Pickup();
		}
	}
	return value;
}

bool EP1_GameplayObject::DoDrop() {
	bool value = false;
	if (pHeldObject) {
		value = Drop();
		pHeldObject = nullptr;
	}
	return value;
}

bool EP1_GameplayObject::DoMoved() {
	bool value = false;
	if (pHeldObject) {
		value = Moved();
	}
	return value;
}

// Damage Precuror

void EP1_GameplayObject::DoTakeDamage(cCombatantPtr target, float damage, cCombatantPtr attacker) {
	if (IsHandledObject(object_cast<cSpatialObject>(target))) {
		OnDamaged(target, damage, attacker);
	}
}

//-----------------------------------------------------------------------------------------------
// Virtuals - Object Definition

// Return true if an object is one that is handled by this class.
// Pure virtual, must be overriden
/*
bool EP1_GameplayObject::IsHandledObject(cSpatialObjectPtr object) {
	return false;
}*/

bool EP1_GameplayObject::IsValidCombatantType(cCombatantPtr combatant) {
	if (mbCombatantCanBeCreature && object_cast<cCreatureAnimal>(combatant)) { return true; }
	if (mbCombatantCanBeVehicle && object_cast<cVehicle>(combatant)) { return true; }
	if (mbCombatantCanBeBuilding && object_cast<cBuilding>(combatant)) { return true; }
	if (mbCombatantCanBeMisc) { return true; }
	return false;
}

float EP1_GameplayObject::GetObjectMaxRadius(cSpatialObjectPtr object) {
	return 4.0f * object->GetScale();
}

Vector3 EP1_GameplayObject::GetObjectPos(cSpatialObjectPtr object) {
	return object->GetPosition();
}

//-----------------------------------------------------------------------------------------------
// Virtuals - Actions from Manager

void EP1_GameplayObject::OnDamaged(cCombatantPtr object, float damage, cCombatantPtr pAttacker) {
	return;
};

void EP1_GameplayObject::OnEnterRadius(cSpatialObjectPtr object, cCombatantPtr pActivator) {
	ApplyCombatantEffect(pActivator, object);
}

void EP1_GameplayObject::OnExitRadius(cSpatialObjectPtr object, cCombatantPtr pActivator) {
	ResetCombatantEffect(pActivator);
}

//-----------------------------------------------------------------------------------------------
// Virtuals - Actions from Manager

void EP1_GameplayObject::ApplyAllObjectEffects() {
	StoreObjects();
	for (auto item : mpIngameObjects) {
		ApplyObjectEffect(item.object);
	}
}

void EP1_GameplayObject::ApplyObjectEffect(cSpatialObjectPtr object) {
	auto combatant = GetClosestCombatant(object);
	if (combatant) {
		ApplyCombatantEffect(combatant, object);
	}
	else {
		for (auto item : mpIngameObjects) {
			if (item.object == object && item.activator) {
				ResetCombatantEffect(item.activator);
				return;
			}
		}
	}
	
}

// formerly pure virtual, should be overriden
void EP1_GameplayObject::ApplyCombatantEffect(cCombatantPtr combatant, cSpatialObjectPtr object) {
}
void EP1_GameplayObject::ResetCombatantEffect(cCombatantPtr combatant) {
}


// Inputs
bool EP1_GameplayObject::Pickup() {
	Moved();
	return false;
}

bool EP1_GameplayObject::Drop() {
	Moved();
	return false;
}

bool EP1_GameplayObject::Moved() {
	auto object = GetClosestHandledObject(pHeldObject);
	if (object) {
		ApplyCombatantEffect(GetHeldCombatant(), object);
	}
	else {
		ResetCombatantEffect(GetHeldCombatant());
	}
	return false;
}

// Messages
void EP1_GameplayObject::Update() {
}

void EP1_GameplayObject::UndoRedo() {
	ApplyAllObjectEffects();
}

void EP1_GameplayObject::SwitchGameMode(bool to_scenario) {
	if (to_scenario) {
		ApplyAllObjectEffects();
	}
}

using namespace App;
void EP1_GameplayObject::EnterMode(int mode) {
	// Playmode
	if (mode == Mode::PlayMode) {
		ApplyAllObjectEffects();
	}
	// Edit / Terrain mode
	else {
		ApplyAllObjectEffects();
	}
}

void EP1_GameplayObject::UpdateScenarioGoals() {
	ApplyAllObjectEffects();
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
