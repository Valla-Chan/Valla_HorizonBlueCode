#include "stdafx.h"
#include "CRG_ObjectManager.h"
#include <Spore\Simulator.h>
#include "CapabilityChecker.h"
#include <Spore/Simulator/cCreatureGameData.h>


cObjectManager::cObjectManager()
{
	App::AddUpdateFunction(this);
}

cObjectManager::~cObjectManager()
{
}

cObjectManager* cObjectManager::sInstance;

cObjectManager* cObjectManager::Get()
{
	return sInstance;
}

void cObjectManager::Update()
{
	if (waiting_for_noun) {
		TestInteractableForDestruction();
		//App::ConsolePrintF("Begin waiting");
	}
}


cInteractiveOrnament* cObjectManager::GetNearestObject() const {
	cCreatureAnimal* avatar = GameNounManager.GetAvatar();
	if (!avatar) { return nullptr; }
	
	float last_distance = 4096.0f;
	cInteractiveOrnament* object_nearest = nullptr;

	// loop through all ingame interactables
	auto interactables = GetData<cInteractiveOrnament>();
	for (auto object : interactables) {
		float dist = (avatar->GetPosition() - object->GetPosition()).SquaredLength();
		if (dist < last_distance) {
			object_nearest = object.get();
			last_distance = dist;
		}
	}
	// DEV TEST
	//object_nearest->SetScale(4);
	return object_nearest;
}

cInteractiveOrnament* cObjectManager::FindInteractedObject() {
	cCreatureBase* avatar = object_cast<Simulator::cCreatureBase>(GameNounManager.GetAvatar());
	if (!avatar) { return nullptr; }

	float last_distance = 4096.0f;
	cInteractiveOrnament* object_nearest = nullptr;

	// loop through all ingame interactables
	auto interactables = GetData<cInteractiveOrnament>();
	for (auto object : interactables) {
		if (object->mpWhoIsInteractingWithMe == avatar) {
			object_nearest = object.get();
		}
	}

	return object_nearest;
}

void cObjectManager::SetInteractedObject(cInteractiveOrnament* object) {
	last_object = object;
	last_object_model = object->GetModelKey();
}

void cObjectManager::ClearInteractedObject() {
	last_object = nullptr;
	last_object_model = { 0,0,0 };
	waiting_for_noun = false;
}

bool cObjectManager::HasModelChanged() const {
	return (last_object->GetModelKey() != last_object_model);
}

void cObjectManager::StartWaitingForNoun() {
	waiting_for_noun = true;
}


// Test if the model has been destroyed (TODO: changed to ONLY its final, destroyed model)
// and enact the model rewards upon the avatar.
void cObjectManager::TestInteractableForDestruction() {
	if (last_object && HasModelChanged()) {
		cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
		if (IsCarnivore(avatar)) {
			ApplyModelRewards(avatar, last_object_model);
		}
		else {
			avatar->mpAnimatedCreature->PlayAnimation(0x931FDCDA); // mot_fruit_eat_chew_vom
		}

		ClearInteractedObject();
	}
}

// Apply rewards from the model onto a creature
void cObjectManager::ApplyModelRewards(const cCreatureBasePtr& creature, const ResourceKey& modelKey) {
	bool success = DoesCreatureSucceedModel(creature, modelKey);
	
	if (success) {
		// Read Reward values
		float health = GetModelHealthReward(modelKey);
		float food = GetModelFoodReward(modelKey);
		float dna = GetModelDNAReward(modelKey);

		// Apply values
		if (health != 0) { creature->SetHealthPoints(creature->mHealthPoints + health); }
		if (food != 0) { creature->mHunger += food; }
		if (dna != 0) { Simulator::cCreatureGameData::AddEvolutionPoints(dna); }

		ResourceKey animkey = GetModelSuccessAnim(modelKey);
		if (animkey.instanceID != 0x0) {
			creature->mpAnimatedCreature->PlayAnimation(0x931FDCDA);
		}
	}
	// Failure
	else {
		// Read Penalty values
		float health = GetModelHealthPenalty(modelKey);
		float food = GetModelFoodPenalty(modelKey);
		float dna = GetModelDNAPenalty(modelKey);

		// Apply values
		if (health != 0) { creature->SetHealthPoints(creature->mHealthPoints - health); }
		if (food != 0) { creature->mHunger -= food; }
		if (dna != 0) { Simulator::cCreatureGameData::AddEvolutionPoints(-1*dna); }

		ResourceKey animkey = GetModelFailureAnim(modelKey);
		if (animkey.instanceID != 0x0) {
			creature->mpAnimatedCreature->PlayAnimation(0x931FDCDA);
		}
	}
	
	
}

//------------------------------
// Capability / Property Checks
//------------------------------

// TODO: is this still needed?
bool cObjectManager::IsCarnivore(const cCreatureBasePtr& creature) const {
	if (creature) {
		return CapabilityChecker.GetCapabilityLevel(creature, 0x022E7847) > 0;
	}
	else { return false; }
}

// Return if the creature matches ALL the object's criteria.
bool cObjectManager::DoesCreatureSucceedModel(const cCreatureBasePtr& creature, const ResourceKey& modelKey) const {
	int value = 0;
	value += !MatchesProperty(0x022E7847, creature, modelKey); // carn
	value += !MatchesProperty(0x022E785C, creature, modelKey); // herb

	return value == 0;
}

// Return if the creature matches the object's single property criteria.
bool cObjectManager::MatchesProperty(const uint32_t property, const cCreatureBasePtr& creature, const ResourceKey& modelKey) const {
	return CapabilityChecker.GetCapabilityLevel(creature, property) > CapabilityChecker.GetModelIntValue(modelKey, property);
}


// Open a model resource and find what anim it wants the avatar to use
ResourceKey cObjectManager::GetModelInteractAnim(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelKeyValue(modelKey, id("modelInteractAnim"));
}
ResourceKey cObjectManager::GetModelSuccessAnim(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelKeyValue(modelKey, id("modelSuccessAnim"));
}
ResourceKey cObjectManager::GetModelFailureAnim(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelKeyValue(modelKey, id("modelFailureAnim"));
}

// Rewards
float cObjectManager::GetModelHealthReward(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelFloatValue(modelKey, id("modelDNAReward"));
}
float cObjectManager::GetModelFoodReward(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelFloatValue(modelKey, id("modelFoodReward"));
}
float cObjectManager::GetModelDNAReward(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelFloatValue(modelKey, id("modelDNAReward"));
}
// Penalties
float cObjectManager::GetModelHealthPenalty(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelFloatValue(modelKey, id("modelDNAPenalty"));
}
float cObjectManager::GetModelFoodPenalty(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelFloatValue(modelKey, id("modelFoodPenalty"));
}
float cObjectManager::GetModelDNAPenalty(const ResourceKey& modelKey) const {
	return CapabilityChecker.GetModelFloatValue(modelKey, id("modelDNAPenalty"));
}


//------------------------------




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
