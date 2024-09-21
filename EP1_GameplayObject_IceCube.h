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
	//const ColorRGB frozenColor = ColorRGB(2.4f, 2.5f, 2.6f);
	const ColorRGB frozenColor = ColorRGB(1.2f, 1.4f, 1.6f);

	void BreakIce(cSpatialObjectPtr object);
	void SetCreatureFrozen(cCreatureAnimalPtr creature, bool frozen);


};
