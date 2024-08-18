#pragma once

#include <Spore\BasicIncludes.h>

class CRG_Recharge 
	: public ArgScript::ICommand
{
public:
	CRG_Recharge();
	~CRG_Recharge();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};

