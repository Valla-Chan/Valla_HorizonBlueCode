#include "stdafx.h"
#include "Cure.h"

Cure::Cure()
{
}


Cure::~Cure()
{
}


void Cure::ParseLine(const ArgScript::Line& line)
{
	// This method is called when your cheat is invoked.
	// Put your cheat code here.
}

const char* Cure::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat does something.";
	}
	else {
		return "Cure: Elaborate description of what this cheat does.";
	}
}
