#include "stdafx.h"
#include "EP1_GameplayObject_HoloProjector.h"
#include "CapabilityChecker.h"

EP1_GameplayObject_HoloProjector::EP1_GameplayObject_HoloProjector()
{
	App::AddUpdateFunction(this);
}

EP1_GameplayObject_HoloProjector::~EP1_GameplayObject_HoloProjector()
{
}

void EP1_GameplayObject_HoloProjector::Update()
{
	if (IsScenarioMode()) {
		if (ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode) {
			for (auto creature : mHolograms) {
				if (creature) {
					auto projector = GetClosestProjector(creature);
					if (projector) {
						SnapToProjector(creature, projector);
						MakeObjectHologram(creature, projector, true);
					}
				}
			}
		}
		else if (in_playmode && ScenarioMode.GetMode() == App::cScenarioMode::Mode::EditMode) {
			ApplyHologramsToProjectors(false);
			in_playmode = false;
		}
	}
}


cCreatureAnimalPtr EP1_GameplayObject_HoloProjector::GetSelectedCreature() const {
	auto objects = Simulator::GetDataByCast<Simulator::cCreatureAnimal>();
	for (auto object : objects) {
		if (object->IsSelected()) {
			return object;
		}
	}
	return nullptr;
}

cCreatureAnimalPtr EP1_GameplayObject_HoloProjector::GetRolledCreature() const {
	auto objects = Simulator::GetDataByCast<Simulator::cCreatureAnimal>();
	for (auto object : objects) {
		if (object->IsRolledOver()) {
			return object;
		}
	}
	return nullptr;
}

// get the closest hologram projector
cSpatialObjectPtr EP1_GameplayObject_HoloProjector::GetClosestProjector(cCreatureAnimalPtr creature) const {
	auto origin = creature->GetPosition();
	float last_dist = max_holo_dist;
	cSpatialObjectPtr found_object = nullptr;
	// do not allow avatar to be a hologram
	if (creature == GameNounManager.GetAvatar()) {
		return nullptr;
	}

	auto objects = Simulator::GetDataByCast<Simulator::cSpatialObject>();
	for (auto object : objects) {
		auto powerup = object_cast<cScenarioPowerup>(object);
		// check if this is a projector
		if (powerup && CapabilityChecker.GetModelColorRGBValue(object->GetModelKey(), id("modelHologramColor")) != ColorRGB(0.0f, 0.0f, 0.0f)) {
			float dist = Math::distance(origin, GetHologramPos(object));
			if (dist < last_dist * object->GetScale()) {
				found_object = object;
				last_dist = dist;
			}
		}
	}
	return found_object;
}

// get the closest creature to a hologram projector
cCreatureAnimalPtr EP1_GameplayObject_HoloProjector::GetClosestCreature(cSpatialObjectPtr projector) const {
	auto origin = GetHologramPos(projector);
	float last_dist = max_holo_dist * projector->GetScale();
	cCreatureAnimalPtr found_creature = nullptr;
	auto avatar = GameNounManager.GetAvatar();

	auto creatures = Simulator::GetDataByCast<Simulator::cCreatureAnimal>();
	for (auto creature : creatures) {
		float dist = Math::distance(origin, creature->GetPosition()); //GetHologramPos(creature)
		float dist2 = Math::distance(origin, GetHologramPos(creature));
		if (dist < last_dist && creature != avatar) {
			found_creature = creature;
			last_dist = dist;
		}
		else if (dist2 < last_dist && creature != avatar) {
			found_creature = creature;
			last_dist = dist2;
		}
	}
	return found_creature;
}

// get position to place the hologram creature at, based on the projector.
Vector3 EP1_GameplayObject_HoloProjector::GetHologramPos(cSpatialObjectPtr projector) const {
	Vector3 pos = projector->GetPosition();
	Vector3 vec_up = pos.Normalized();
	return pos + (vec_up * holo_zdist * projector->GetScale());
}

void EP1_GameplayObject_HoloProjector::MakeObjectHologram(cCreatureAnimalPtr creature, cSpatialObjectPtr projector, bool state) {
	if (state) {
		if (creature) {
			creature->mbStealthed = true;
			creature->field_B7C = 0.4f;
			if (projector) {
				auto color = CapabilityChecker.GetModelColorRGBValue(projector->GetModelKey(), id("modelHologramColor"));
				creature->SetIdentityColor(color);
			}
			creature->StopMovement();
			//mHolograms.push_back(creature);
		}
	}
	else {
		if (creature) {
			creature->mbStealthed = false;
			creature->field_B7C = 1.0f;
			creature->SetIdentityColor(ColorRGB(1.0f, 1.0f, 1.0f));
			//for (size_t i = 0; i < mHolograms.size(); i++) {
			//	if (mHolograms[i] == creature) {
			//		mHolograms.
			//	}
			//}
		}
	}
}

void EP1_GameplayObject_HoloProjector::SnapToProjector(cCreatureAnimalPtr creature, cSpatialObjectPtr projector) {
	Vector3 newpos = GetHologramPos(projector);
	creature->mbSupported = true;
	creature->Teleport(newpos, creature->GetOrientation());
	creature->mbSupported = true;
}

eastl::vector<cSpatialObjectPtr> EP1_GameplayObject_HoloProjector::GetProjectors() const {
	eastl::vector<cSpatialObjectPtr> projectors = {};

	auto objects = Simulator::GetDataByCast<Simulator::cSpatialObject>();
	for (auto object : objects) {
		auto powerup = object_cast<cScenarioPowerup>(object);
		if (powerup && CapabilityChecker.GetModelColorRGBValue(object->GetModelKey(), id("modelHologramColor")) != ColorRGB(0.0f, 0.0f, 0.0f)) {
			projectors.push_back(object);
		}
	}
	return projectors;
}

//-----------------------------------------------------------------------------------------------

bool EP1_GameplayObject_HoloProjector::IsCreatureHologram(cCreatureAnimalPtr creature) const {
	if (creature->GetIdentityColor() == ColorRGB(1, 1, 1) || creature->field_B7C == 1.0f) { return false; }
	for (auto item : mHolograms) {
		if (item == creature) {
			return true;
		}
	}
	return false;
}

// apply all nearby creatures to the projectors. run this when going into playmode
void EP1_GameplayObject_HoloProjector::ApplyHologramsToProjectors(bool playmode) {
	mHolograms.clear();
	for (auto projector : GetProjectors()) {
		auto creature = GetClosestCreature(projector);
		if (creature) {
			MakeObjectHologram(creature, projector);
			SnapToProjector(creature, projector);
			if (playmode) {
				in_playmode = true;
				creature->mStandardSpeed = 0.0f;
				creature->mTurnRate = 0.0f;
				mHolograms.push_back(creature);
			}
		}
	}
}



//-----------------------------------------------------------------------------------------------

void EP1_GameplayObject_HoloProjector::Pickup() {
	selectedCreature = GetRolledCreature();
	if (selectedCreature) {
		auto projector = GetClosestProjector(selectedCreature);
		if (projector) {
			MakeObjectHologram(selectedCreature, projector);
		}
	}
}
void EP1_GameplayObject_HoloProjector::Drop() {

	// if near a hologram projector when dropped, teleport the creature above it.
	auto holo = GetClosestProjector(selectedCreature);
	if (holo) {
		SnapToProjector(selectedCreature, holo);
	}
	selectedCreature = nullptr;
}

void EP1_GameplayObject_HoloProjector::Moved() {
	// make the hologram state be set by the creature's proximity to the holo projector
	if (selectedCreature) {
		auto projector = GetClosestProjector(selectedCreature);
		MakeObjectHologram(selectedCreature, projector, projector);
	}
}



//-----------------------------------------------------------------------------------------------

void EP1_GameplayObject_HoloProjector::UndoRedo()
{
	ApplyHologramsToProjectors(false);
}

void EP1_GameplayObject_HoloProjector::FireIfScenario() {
	if (IsScenarioMode()) {
		ApplyHologramsToProjectors(false);
	}
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
	CLASS_CAST(IWinProc);
	CLASS_CAST(EP1_GameplayObject_HoloProjector);
	return nullptr;
}