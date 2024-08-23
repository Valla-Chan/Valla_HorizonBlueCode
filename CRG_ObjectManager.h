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

	cInteractiveOrnament* GetHoveredObject() const;
	cInteractiveOrnament* GetNearestObject() const;
	cInteractiveOrnament* FindInteractedObject();

	void SetInteractedObject(cInteractiveOrnament* object);
	void ClearInteractedObject();
	bool HasModelChanged() const;
	void StartWaitingForNoun();
	void TestInteractableForDestruction();

	void ApplyModelRewards(const cCreatureBasePtr& creature, const ResourceKey& modelKey);
	bool DoesCreatureSucceedModel(const cCreatureBasePtr& creature, const ResourceKey& modelKey) const;
	bool MatchesProperty(const uint32_t property, const cCreatureBasePtr& creature, const ResourceKey& modelKey) const;

	ResourceKey GetModelInteractAnim(const cCreatureBasePtr& creature, const ResourceKey& modelKey, const uint32_t default_animID);
	ResourceKey GetModelSuccessAnim(const ResourceKey& modelKey) const;
	ResourceKey GetModelFailureAnim(const ResourceKey& modelKey) const;

	uint32_t GetModelCursorID(const ResourceKey& modelKey, const uint32_t default_ID) const;
	float GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property) const;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

protected:
	static cObjectManager* sInstance;

private:

	//intrusive_ptr<Simulator::ScheduledTaskListener> mWaitTask;
};
