#pragma once

#include <Spore\BasicIncludes.h>

class CRG_GiveAllParts 
	: public ArgScript::ICommand
{
public:
	CRG_GiveAllParts();
	~CRG_GiveAllParts();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	void UnlockParts();
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
private:
	const char* mParameter;
};

