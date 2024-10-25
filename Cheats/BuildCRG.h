#pragma once

#include <Spore\BasicIncludes.h>

class BuildCRG 
	: public ArgScript::ICommand
	, public DefaultRefCounted
{
public:
	BuildCRG();
	~BuildCRG();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	void SetNestModel();
	virtual int AddRef() override;
	virtual int Release() override;
	bool CompareStrings(const char* str1, const char* str2);
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
private:
	const char* mParameter;
	Clock clocktest;
};

