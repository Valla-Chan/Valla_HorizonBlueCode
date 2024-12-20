#pragma once

#include <Spore\BasicIncludes.h>

using namespace Editors;
using namespace Simulator;
using namespace UI;
class HBdebug 
	: public ArgScript::ICommand
{
public:
	HBdebug();
	~HBdebug();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};

