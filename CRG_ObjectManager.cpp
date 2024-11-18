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
	return object_nearest;
}

cInteractiveOrnament* cObjectManager::GetHoveredObject() const {
	cCreatureAnimal* avatar = GameNounManager.GetAvatar();
	if (!avatar) { return nullptr; }

	/* LEGACY
	// loop through all ingame interactables
	auto interactables = GetData<cInteractiveOrnament>();
	for (auto object : interactables) {
		if (object->IsRolledOver()) { return object.get(); }
	}*/
	auto object = GameViewManager.GetHoveredObject();
	auto interactable = object_cast<cInteractiveOrnament>(object);

	return interactable;
}

cCreatureCitizen* cObjectManager::GetNearestTribalLeader(cSpatialObject* object, const float within) const {
	if (!IsTribeGame()) { return nullptr; }

	float last_distance = within;
	cCreatureCitizen* closest_leader = nullptr;

	auto tribes = Simulator::GetDataByCast<Simulator::cTribe>();
	for (auto tribe : tribes) {
		cCreatureCitizen* leader = tribe->GetLeaderCitizen();
		float dist = (leader->GetPosition() - object->mPosition).SquaredLength();
		if (dist < last_distance) {
			closest_leader = leader;
		}
	}
	return closest_leader;
}

cInteractiveOrnament* cObjectManager::FindInteractedObject() {
	cCreatureBase* avatar = object_cast<Simulator::cCreatureBase>(GameNounManager.GetAvatar());
	if (!avatar) { return nullptr; }
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
	last_object_model = ResourceKey();
	waiting_for_noun = false;
	interaction_success = false;
}

bool cObjectManager::HasModelChanged() const {
	// return if model has changed or fade has started
	return (last_object->GetModelKey() != last_object_model) || last_object->mFadeTimer > 1;
}

void cObjectManager::StartWaitingForNoun() {
	waiting_for_noun = true;
}


// Test if the model has been destroyed (TODO: changed to ONLY its final, destroyed model)
// and enact the model rewards upon the avatar.
void cObjectManager::TestInteractableForDestruction() {
	if (last_object && HasModelChanged()) {
		cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();
		ApplyModelRewards(avatar, last_object_model, interaction_success);

		ClearInteractedObject();
	}
}

// Apply rewards/penalties from the model onto a creature
void cObjectManager::ApplyModelRewards(const cCreatureBasePtr& creature, const ResourceKey& modelKey, bool success) {
	if (success) {

		// Read Reward values
		//---------------------------------------------------------------------------------
		float health = GetModelFloatValue(modelKey, id("modelHealthReward"));
		float food = GetModelFloatValue(modelKey, id("modelFoodReward"));
		float dna = GetModelFloatValue(modelKey, id("modelDNAReward"));

		// Part Rewards
		Vector2* partLevels = {};
		size_t numParts = 0;

		PropertyListPtr mpPropList;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			bool test = App::Property::GetArrayVector2(mpPropList.get(), id("modelPartsReward"), numParts, partLevels);
			if (!test) {
				numParts = 0;
			}
		}
		//---------------------------------------------------------------------------------

		// Apply values
		if (health != 0.0f) { creature->SetHealthPoints(creature->mHealthPoints + health); }
		if (food != 0.0f) {
			creature->mHunger += food;
			// reset the food timer if this adds to the food value.
			if (food > 0) {
				LARGE_INTEGER b = { 0 };
				creature->mHungerDelayTimer.SetTime(b);
				creature->mHungerDelayTimer.Start();
			}
		}
		if (dna != 0.0f) { Simulator::cCreatureGameData::AddEvolutionPoints(dna); }
		if (numParts > 0 && partLevels) {
			for (size_t i = 0; i < numParts; i++) {
				int minValue = int(min(floor(partLevels[i].x), floor(partLevels[i].y)));
				int maxValue = int(max(floor(partLevels[i].x), floor(partLevels[i].y)));

				// randomize part level
				int level = minValue + rand(maxValue - minValue + 1); // add 1 in order to include last value

				// Unlock a part in this range.
				App::CreatureModeStrategies::UnlockPart action = { GameNounManager.GetAvatar(), 0, level };
				CreatureModeStrategy.ExecuteAction(action.ID, &action);
			}
		}

		auto animID = GetModelSuccessAnim(modelKey);
		if (animID != 0x0) {
			creature->mpAnimatedCreature->PlayAnimation(animID);
		}
	}
	// Failure
	else {
		// Read Penalty values
		//---------------------------------------------------------------------------------
		float health = GetModelFloatValue(modelKey, id("modelHealthPenalty"));
		float food = GetModelFloatValue(modelKey, id("modelFoodPenalty"));
		float dna = GetModelFloatValue(modelKey, id("modelDNAPenalty"));
		float disease = GetModelBoolValue(modelKey, id("modelSicknessPenalty"));
		//---------------------------------------------------------------------------------

		// Apply values
		if (health != 0.0f) { creature->SetHealthPoints(creature->mHealthPoints - health); }
		if (food != 0.0f) { creature->mHunger -= food; }
		if (dna != 0.0f) { Simulator::cCreatureGameData::AddEvolutionPoints(-1*dna); }
		if (disease) { creature->mbIsDiseased = true; }

		auto animID = GetModelFailureAnim(modelKey);
		if (animID != 0x0) {
			creature->mpAnimatedCreature->PlayAnimation(animID);
		}
	}
	
	
}

//------------------------------
// Capability / Property Checks
//------------------------------

// Return if the creature matches/surpasses ALL the object's criteria.
bool cObjectManager::DoesCreatureSucceedModel(const cCreatureBasePtr& creature, const ResourceKey& modelKey) {
	int value = 0;
	value += !MatchesProperty(0x022E7847, creature, modelKey); // carn
	value += !MatchesProperty(0x022E785C, creature, modelKey); // herb

	return value == 0;
}

// Return if the creature matches or surpassed the object in one property criteria.
bool cObjectManager::MatchesProperty(const uint32_t property, const cCreatureBasePtr& creature, const ResourceKey& modelKey) {
	return CapabilityChecker.GetCapabilityLevel(creature, property) >= CapabilityChecker.GetModelIntValue(modelKey, property);
}


// Open a model resource and get a float value from a property
float cObjectManager::GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property) {
	return CapabilityChecker.GetModelFloatValue(modelKey, property);
}

// Open a model resource and get a bool value from a property
bool cObjectManager::GetModelBoolValue(const ResourceKey& modelKey, const uint32_t property) {
	return CapabilityChecker.GetModelBoolValue(modelKey, property);
}

// Open a model resource and get an array of vector2 values of a property
Vector2* cObjectManager::GetModelVector2sValue(const ResourceKey& modelKey, const uint32_t property) {
	return CapabilityChecker.GetModelVector2sValue(modelKey, property);
}

// NONSTATIC. 
// Calculate success var and return anim to use.
uint32_t cObjectManager::ChooseModelInteractSuccessFailureAnim(const cCreatureBasePtr& creature, const ResourceKey& modelKey, const uint32_t default_animID) {
	interaction_success = DoesCreatureSucceedModel(creature, modelKey);
	return GetModelInteractAnim(modelKey, default_animID, interaction_success);
}

// Open a model resource and find what anim it wants the avatar to use when first interacting
uint32_t cObjectManager::GetModelInteractAnim(const ResourceKey& modelKey, const uint32_t default_animID, bool success) {
	// Succeeded, or has no failure anim
	if (success || !CapabilityChecker.HasModelKeyValue(modelKey, id("modelInteractFailureAnim"))) {
		if (CapabilityChecker.HasModelKeyValue(modelKey, id("modelInteractAnim"))) {
			return CapabilityChecker.GetModelKeyValue(modelKey, id("modelInteractAnim")).instanceID;
		}
	}
	// Failed and has special anim
	else if (!success && CapabilityChecker.HasModelKeyValue(modelKey, id("modelInteractFailureAnim"))) {
		return CapabilityChecker.GetModelKeyValue(modelKey, id("modelInteractFailureAnim")).instanceID;
	}
	return default_animID;
}

uint32_t cObjectManager::GetModelSuccessAnim(const ResourceKey& modelKey) {
	return CapabilityChecker.GetModelKeyValue(modelKey, id("modelSuccessAnim")).instanceID;
}
uint32_t cObjectManager::GetModelFailureAnim(const ResourceKey& modelKey) {
	return CapabilityChecker.GetModelKeyValue(modelKey, id("modelFailureAnim")).instanceID;
}

uint32_t cObjectManager::GetModelCursorID(const ResourceKey& modelKey, const uint32_t default_ID) {
	if (CapabilityChecker.HasModelUInt32Value(modelKey, id("modelCursorID"))) {
		return CapabilityChecker.GetModelUInt32Value(modelKey, id("modelCursorID"));
	}
	return default_ID;
}

ResourceKey cObjectManager::GetHerdNestModel(const uint32_t herdID) {
	ResourceKey herd_resource = ResourceKey(herdID, TypeIDs::Names::prop, 0x02f98b67);
	if (CapabilityChecker.HasModelKeyValue(herd_resource, id("NestModelKey"))) {
		return CapabilityChecker.GetModelKeyValue(herd_resource, id("NestModelKey"));
	}
	return ResourceKey{ 0, 0, 0 };
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
