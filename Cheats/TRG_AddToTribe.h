#pragma once

#include <Spore\BasicIncludes.h>

using namespace Simulator;
class TRG_AddToTribe 
	: public ArgScript::ICommand
{
public:
	TRG_AddToTribe();
	~TRG_AddToTribe();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	void AddToTribe(cCreatureCitizenPtr creature, cTribe* tribe);
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};

