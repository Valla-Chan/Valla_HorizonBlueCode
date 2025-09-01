#pragma once

#include <Spore\BasicIncludes.h>

class CVG_SetCityType 
	: public ArgScript::ICommand
{
public:
	CVG_SetCityType();
	~CVG_SetCityType();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
private:
	const char* mParameter;
};

