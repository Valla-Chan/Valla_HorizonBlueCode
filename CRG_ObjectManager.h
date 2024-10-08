#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\cInteractiveOrnament.h>
#include <Spore\App\IMessageListener.h>

#define ObjectManager  (*cObjectManager::Get())
#define cObjectManagerPtr intrusive_ptr<cObjectManager>

using namespace Simulator;

class cObjectManager
	: public Object
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("cObjectManager");
	cObjectManager();
	~cObjectManager();

	void Update() override;
	
	static cObjectManager* Get();

	//--------------------
	// object models
	const uint32_t mdl_carcass01 = id("CRG_Food_Carcass_01");
	// animations
	const uint32_t anim_dig = 0x03DF6DFF;
	const uint32_t anim_eat_meat_mouth = 0x04F65995;
	//--------------------

	bool interaction_success = false;
	bool waiting_for_noun = false;
	cInteractiveOrnamentPtr last_object;
	ResourceKey last_object_model;

	//void ChieftainDied(const cCreatureBasePtr& creature);

	cInteractiveOrnament* GetHoveredObject() const;
	cInteractiveOrnament* GetNearestObject() const;
	cCreatureCitizen* cObjectManager::GetNearestTribalLeader(cSpatialObject* object, const float within = 256.0f) const;
	cInteractiveOrnament* FindInteractedObject();

	void SetInteractedObject(cInteractiveOrnament* object);
	void ClearInteractedObject();
	bool HasModelChanged() const;
	void StartWaitingForNoun();
	void TestInteractableForDestruction();

	static void ApplyModelRewards(const cCreatureBasePtr& creature, const ResourceKey& modelKey, bool success);
	static bool DoesCreatureSucceedModel(const cCreatureBasePtr& creature, const ResourceKey& modelKey);
	static bool MatchesProperty(const uint32_t property, const cCreatureBasePtr& creature, const ResourceKey& modelKey);

	uint32_t ChooseModelInteractSuccessFailureAnim(const cCreatureBasePtr& creature, const ResourceKey& modelKey, const uint32_t default_animID);
	static uint32_t GetModelInteractAnim(const ResourceKey& modelKey, const uint32_t default_animID, bool success);
	static uint32_t GetModelSuccessAnim(const ResourceKey& modelKey);
	static uint32_t GetModelFailureAnim(const ResourceKey& modelKey);

	static uint32_t GetModelCursorID(const ResourceKey& modelKey, const uint32_t default_ID);
	static float GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property);
	static bool GetModelBoolValue(const ResourceKey& modelKey, const uint32_t property);
	static Vector2* GetModelVector2sValue(const ResourceKey& modelKey, const uint32_t property);

	static ResourceKey GetHerdNestModel(const uint32_t herdID);

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

protected:
	static cObjectManager* sInstance;

private:

	//intrusive_ptr<Simulator::ScheduledTaskListener> mWaitTask;
};
