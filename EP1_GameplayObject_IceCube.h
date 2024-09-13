#pragma once

#include "EP1_GameplayObject.h"

#define EP1_GameplayObject_IceCubePtr intrusive_ptr<EP1_GameplayObject_IceCube>

class EP1_GameplayObject_IceCube 
	: public EP1_GameplayObject
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObject_IceCube");
	
	EP1_GameplayObject_IceCube();
	~EP1_GameplayObject_IceCube();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

	//----------------------------------------

	void Update() override;
	bool IsHandledObject(cSpatialObjectPtr object) const override;

	void ApplyCombatantEffect(cCombatantPtr combatant, cSpatialObjectPtr object) override;
	void ResetCombatantEffect(cCombatantPtr combatant) override;

	void OnDamaged(cCombatantPtr object, float damage, cCombatantPtr pAttacker) override;

	//----------------------------------------

	void BreakIce(cSpatialObjectPtr object);

	void SetCreatureFrozen(cCreatureAnimalPtr creature, bool frozen);


private:
	// List of all frozen creatures:
	eastl::vector<cCreatureAnimalPtr> mFrozenCreatures = {};
	eastl::vector<Vector3> mFrozenCreaturePositions = {};
};
