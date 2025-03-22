#pragma once

#include <Spore\BasicIncludes.h>

class HurtTarget 
	: public ArgScript::ICommand
{
public:
	HurtTarget();
	~HurtTarget();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	cCreatureCitizenPtr TraceHitCitizen() const;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
private:
	const char* mParameter;
};

