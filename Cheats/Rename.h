#pragma once

#include <Spore\BasicIncludes.h>

using namespace Simulator;
class Rename 
	: public ArgScript::ICommand
{
public:
	Rename();
	~Rename();

	cCreatureCitizenPtr TraceHitCitizen() const;

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
private:
	const char* mParameter;
};

