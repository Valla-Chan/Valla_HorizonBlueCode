#pragma once

#include <Spore\BasicIncludes.h>

class TRG_SelectMembers 
	: public ArgScript::ICommand
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	TRG_SelectMembers();
	~TRG_SelectMembers();

	int AddRef() override { return DefaultRefCounted::AddRef(); };
	int Release() override { return DefaultRefCounted::Release(); };

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};

