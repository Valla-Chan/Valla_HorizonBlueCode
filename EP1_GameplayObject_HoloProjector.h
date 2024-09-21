#pragma once

#include "EP1_GameplayObject.h"

#define EP1_GameplayObject_HoloProjectorPtr intrusive_ptr<EP1_GameplayObject_HoloProjector>

class EP1_GameplayObject_HoloProjector
	: public EP1_GameplayObject
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObject_HoloProjector");

	EP1_GameplayObject_HoloProjector();
	~EP1_GameplayObject_HoloProjector();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

	const float max_holo_dist = 2.5f;
	const float holo_zdist = 2.0f;

	bool mbReapplyEffect = true;

	//eastl::vector<cCreatureAnimalPtr> mHolograms = {};
	//-------------------------------------------------------------

	void Update() override;
	bool IsHandledObject(cSpatialObjectPtr object) const override;
	
	Vector3 GetObjectPos(cSpatialObjectPtr object) override;
	float GetObjectMaxRadius(cSpatialObjectPtr object) override;

	void ApplyCombatantEffect(cCombatantPtr combatant, cSpatialObjectPtr object) override;
	void ResetCombatantEffect(cCombatantPtr combatant) override;

	//void OnActivatorDamaged(cCombatantPtr object, float damage, cCombatantPtr pAttacker) override;

	bool MouseClick() override;

	//-------------------------------------------------------------

	//void DeselectRolled();

	void MakeCreatureHologram(cCreatureAnimalPtr creature, cSpatialObjectPtr projector = nullptr, bool state = true);
	void SnapToProjector(cCreatureAnimalPtr creature, cSpatialObjectPtr projector);

	bool IsCreatureHologram(cCreatureAnimalPtr creature) const;

	

};
