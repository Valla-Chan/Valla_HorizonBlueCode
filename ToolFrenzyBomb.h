#pragma once
#include "stdafx.h"
#include <Spore\BasicIncludes.h>
#include <Spore/Simulator/cSpaceToolData.h>
#include <Spore/Simulator/cDefaultProjectileWeapon.h>
/*
#include <Spore/Swarm/IEffectsWorld.h>
#include <Spore/Swarm/IEffectsManager.h>
#include <Spore/Swarm/EffectDirectory.h>
#include <Spore/Swarm/cEffectsManager.h>
#include <Spore/Swarm/cVisualEffect.h>
#include <Spore/Swarm/IVisualEffect.h>
*/


#define ToolFrenzyBombPtr intrusive_ptr<ToolFrenzyBomb>

class ToolFrenzyBomb
	:public Simulator::cDefaultProjectileWeapon
{
public:

	ToolFrenzyBomb();
	~ToolFrenzyBomb();


	bool OnHit(Simulator::cSpaceToolData* pTool, const Vector3& position, Simulator::SpaceToolHit hitType, int) override;
	//virtual bool OnSelect(Simulator::cSpaceToolData* pTool) override;
private:

	enum TargetPref {
		Creature,
		Building,
		Vehicle,
	};

	cCombatantPtr GetNearestFriendlyCombatant(cCreatureBasePtr ref, TargetPref targetpref = Creature);
	float Distance(cCombatantPtr target, cCombatantPtr ref) const;
	void UpdateCombatantFromDistance(cCombatantPtr target, cCombatantPtr ref, float& cdist, cCombatantPtr& ccombatant);
	//void StoreNearestCombatantFromPool(cCombatantPtr ref, vector<cCombatantPtr> pool);
};