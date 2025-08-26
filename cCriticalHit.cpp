#include "stdafx.h"
#include "cCriticalHit.h"

void cCriticalHit::AttachDetours() {
	CombatTakeCritDamage_detour::attach(GetAddress(Simulator::cCombatant, TakeDamage));
};