#pragma once

#include <Spore\BasicIncludes.h>

class CRG_GetPart 
	: public ArgScript::ICommand
{
public:
	CRG_GetPart();
	~CRG_GetPart();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};
