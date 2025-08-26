#pragma once

#include <Spore\BasicIncludes.h>
#include "TargetCheat.h"

class Kill 
	: public ArgScript::ICommand
	, public TargetCheat
{
public:
	Kill();
	~Kill();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	void KillTarget(const cCombatantPtr target);

	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};

