#pragma once

#include <Spore\BasicIncludes.h>

class ListenAllMessages 
	: public DefaultRefCounted
	, public App::IMessageListener
	, public ArgScript::ICommand
{
public:
	ListenAllMessages();
	~ListenAllMessages();

	int AddRef() override;
	int Release() override;

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
	bool HandleMessage(uint32_t messageID, void* message) override;
};

