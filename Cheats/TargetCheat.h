#pragma once

#include <Spore\BasicIncludes.h>

#define TargetCheatPtr intrusive_ptr<TargetCheat>

using namespace Simulator;
using namespace Cell;

class TargetCheat 
{
public:
	vector<cCreatureBasePtr> GetCreatureTargets(bool get_combat_targets = true);
	vector<cCombatantPtr> GetCombatantTargets(bool get_combat_targets = true);
	cCreatureBasePtr GetAvatarCreature();
	cCombatantPtr GetAvatarCombatant();
	cCellObjectData* GetPlayerCell() { GetPlayerCell(); }
};
