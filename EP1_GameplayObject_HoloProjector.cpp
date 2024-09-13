#include "stdafx.h"
#include "EP1_GameplayObject_HoloProjector.h"
#include "CapabilityChecker.h"

EP1_GameplayObject_HoloProjector::EP1_GameplayObject_HoloProjector()
{
}

EP1_GameplayObject_HoloProjector::~EP1_GameplayObject_HoloProjector()
{
}

void EP1_GameplayObject_HoloProjector::Update() {
	if (IsPlayingAdventure()) {
		for (auto item : mpIngameObjects) {
			auto creature = object_cast<cCreatureAnimal>(item.activator);
			if (creature) {
				//MakeCreatureHologram(creature);
				creature->mbStealthed = true;
				creature->field_B7C = 0.4f;
			}
		}
	}
}

bool EP1_GameplayObject_HoloProjector::IsHandledObject(cSpatialObjectPtr object) const {
	auto powerup = object_cast<cScenarioPowerup>(object);
	// check if this is a projector
	if (powerup && CapabilityChecker.GetModelColorRGBValue(object->GetModelKey(), id("modelHologramColor")) != ColorRGB(0.0f, 0.0f, 0.0f)) {
		return true;
	}
	return false;
}

Vector3 EP1_GameplayObject_HoloProjector::GetObjectPos(cSpatialObjectPtr object) {
	Vector3 pos = object->GetPosition();
	Vector3 vec_up = pos.Normalized();
	return pos + (vec_up * holo_zdist * object->GetScale());
}

float EP1_GameplayObject_HoloProjector::GetObjectMaxRadius(cSpatialObjectPtr object) {
	return max_holo_dist * object->GetScale();
}


void EP1_GameplayObject_HoloProjector::MakeCreatureHologram(cCreatureAnimalPtr creature, cSpatialObjectPtr projector, bool state) {
	if (state) {
		if (creature) {
			creature->mbStealthed = true;
			creature->field_B7C = 0.4f;
			if (projector) {
				auto color = CapabilityChecker.GetModelColorRGBValue(projector->GetModelKey(), id("modelHologramColor"));
				creature->SetIdentityColor(color);
			}
			creature->StopMovement();
		}
	}
	else {
		if (creature) {
			creature->mbStealthed = false;
			creature->field_B7C = 1.0f;
			creature->SetIdentityColor(ColorRGB(1.0f, 1.0f, 1.0f));
		}
	}
}

void EP1_GameplayObject_HoloProjector::SnapToProjector(cCreatureAnimalPtr creature, cSpatialObjectPtr projector) {
	Vector3 newpos = GetObjectPos(projector);
	creature->mbSupported = true;
	creature->Teleport(newpos, creature->GetOrientation());
	creature->mbSupported = true;
}

//-----------------------------------------------------------------------------------------------

bool EP1_GameplayObject_HoloProjector::IsCreatureHologram(cCreatureAnimalPtr creature) const {
	if (creature->GetIdentityColor() == ColorRGB(1, 1, 1) || creature->field_B7C == 1.0f) { return false; }
	for (auto item : mpIngameObjects) {
		if (item.activator == creature) {
			return true;
		}
	}
	return false;
}

void EP1_GameplayObject_HoloProjector::ApplyCombatantEffect(cCombatantPtr combatant, cSpatialObjectPtr object) {
	auto creature = object_cast<cCreatureAnimal>(combatant);
	if (creature) {
		MakeCreatureHologram(creature, object);
		if (!IsObjectHeld(object)) {
			SnapToProjector(creature, object);
		}
		if (IsPlayingAdventure()) {
			creature->mStandardSpeed = 0.0f;
			creature->mTurnRate = 0.0f;
			creature->mbFixed = true;
			creature->mbEnabled = false;
			//mHolograms.push_back(creature);
		}
	}
}

void EP1_GameplayObject_HoloProjector::ResetCombatantEffect(cCombatantPtr combatant) {
	auto creature = object_cast<cCreatureAnimal>(combatant);
	MakeCreatureHologram(creature, nullptr, false);
}

//-----------------------------------------------------------------------------------------------



int EP1_GameplayObject_HoloProjector::AddRef()
{
	return DefaultRefCounted::AddRef();
}


int EP1_GameplayObject_HoloProjector::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_GameplayObject_HoloProjector::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(EP1_GameplayObject_HoloProjector);
	return nullptr;
}