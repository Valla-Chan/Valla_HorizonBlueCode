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
	void Update() override;

	const float max_holo_dist = 2.5f;
	const float holo_zdist = 2.0f;

	cCreatureAnimalPtr selectedCreature = nullptr;
	eastl::vector<cCreatureAnimalPtr> mHolograms = {};
	bool in_playmode = false;

	cCreatureAnimalPtr GetSelectedCreature() const;
	cCreatureAnimalPtr GetRolledCreature() const;
	cSpatialObjectPtr GetClosestProjector(cCreatureAnimalPtr creature) const;
	cCreatureAnimalPtr GetClosestCreature(cSpatialObjectPtr projector) const;
	Vector3 GetHologramPos(cSpatialObjectPtr projector) const;
	void MakeObjectHologram(cCreatureAnimalPtr creature, cSpatialObjectPtr projector = nullptr, bool state = true);
	void SnapToProjector(cCreatureAnimalPtr creature, cSpatialObjectPtr projector);

	eastl::vector<cSpatialObjectPtr> GetProjectors() const;
	bool IsCreatureHologram(cCreatureAnimalPtr creature) const;

	void ApplyHologramsToProjectors(bool playmode = false);

	bool Pickup() override;
	bool Drop() override;
	bool Moved() override;

	void UndoRedo();
};
