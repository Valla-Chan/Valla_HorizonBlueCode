#pragma once

#include <Spore\BasicIncludes.h>

#define EP1_GameplayObject_IceCubePtr intrusive_ptr<EP1_GameplayObject_IceCube>

using namespace Simulator;

class EP1_GameplayObject_IceCube 
	: public Object
	, public App::IUpdatable
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObject_IceCube");
	
	EP1_GameplayObject_IceCube();
	~EP1_GameplayObject_IceCube();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;

	static cCreatureAnimalPtr GetClosestCreature(cSpatialObjectPtr gameplayob);
	bool IsObjectIce(cSpatialObjectPtr object) const;
	void BreakIce(cSpatialObjectPtr object);
	eastl::vector<cSpatialObjectPtr> GetIceCubes() const;
	void ApplyFrozenToIce();
	void SetCreatureFrozen(cCreatureAnimalPtr creature, bool frozen);

	bool HandleMessage(uint32_t messageID, void* msg) override;
	void FireIfScenario();


private:
	// List of all frozen creatures:
	eastl::vector<cCreatureAnimalPtr> mFrozenCreatures = {};
	eastl::vector<Vector3> mFrozenCreaturePositions = {};
};
