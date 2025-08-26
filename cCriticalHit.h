#pragma once

#include <Spore\BasicIncludes.h>
#include "CapabilityChecker.h"

#define cCriticalHitPtr intrusive_ptr<cCriticalHit>

using namespace Simulator;

// Stats
static bool always_allowcrit = true; // default = false
static bool always_docrit = false; // default = false
static const vector<float> crit_chances = {
		0.01f, // lvl 0
		0.10f, // lvl 1
		0.22f, // lvl 2
		0.28f, // lvl 3
		0.34f, // lvl 4
		0.37f, // lvl 5
};
//
static const uint32_t crit_effect = 0x57c5d2d7; // Placeholder

class cCriticalHit 
{
public:

	static void AttachDetours();

	static float GetCritDamage(float damage, int critlvl_cr_atkr, int critlvl_cr_trgt) {
		float new_dmg = damage;
		new_dmg = damage * 1.5f;
		if (critlvl_cr_atkr > critlvl_cr_trgt) {
			new_dmg += damage * crit_chances[critlvl_cr_atkr];
		}
		return damage;
	}

	static void PlayCritEffect(cCreatureBasePtr creature) {
		creature->CreateAndStartEffectForPool1(crit_effect);
	}

};

// Called when a combatant takes damage
// TODO: stop health drain from calling this multiple times. this should call one time per ability.
virtual_detour(CombatTakeCritDamage_detour, Simulator::cCombatant, Simulator::cCombatant, int(float, uint32_t, int, const Vector3&, cCombatant*))
{
	int detoured(float damage, uint32_t attackerPoliticalID, int integer, const Vector3 & vector, cCombatant * pAttacker)
	{
		// check if attacker is also a creature
		auto cr_attacker = object_cast<cCreatureBase>(pAttacker);
		// So far, only allow crits on other creatures
		auto cr_target = object_cast<cCreatureBase>(this);

		bool docrit = always_docrit;
		if (cr_attacker && cr_target) {
			bool allowcrit = always_allowcrit;

			int critlvl_cr_atkr = CapabilityChecker::GetCapabilityLevel(cr_attacker, id("modelCapabilityCritical"));
			int critlvl_cr_trgt = CapabilityChecker::GetCapabilityLevel(cr_target, id("modelCapabilityCritical"));

			// check if we should allow crit
			if (!allowcrit) {
				// attacker's crit is above or equal to target's
				// TODO: put in a special exception for epics, which should always be treated as having lvl 5 crit for the sake of defense,
				// but not be able to use crit themselves.
				if (critlvl_cr_atkr >= critlvl_cr_trgt) {
					allowcrit = true;
				}
			}
			// Determine critical chance and set docrit
			if (allowcrit && !docrit) {

				docrit = randf() < crit_chances[critlvl_cr_atkr];
			}

			// If critical is successful, set the new damage and play effects.
			if (docrit) {
				damage = cCriticalHit::GetCritDamage(damage, critlvl_cr_atkr, critlvl_cr_trgt);
				cCriticalHit::PlayCritEffect(cr_target);
			}
			
		}
		

		auto value = original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);

		return value;
	}
};