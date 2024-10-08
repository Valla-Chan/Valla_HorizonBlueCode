#include "stdafx.h"
#include "ListenAllMessages.h"

ListenAllMessages::ListenAllMessages()
{
}


ListenAllMessages::~ListenAllMessages()
{
}

// For internal use, do not modify.
int ListenAllMessages::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ListenAllMessages::Release()
{
	return DefaultRefCounted::Release();
}

void ListenAllMessages::ParseLine(const ArgScript::Line& line)
{
	for (uint32_t value = 0x00000001; value < 0xFFFFFFFE; value++) {
		if (value != 0x0C) {
			MessageManager.AddListener(this, value);
		}
	}
}

const char* ListenAllMessages::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Debug Cheat: Listens for and prints every message in the game. WARNING: SLOW!";
	}
	else {
		return "ListenAllMessages: Debug Cheat: Listens for and prints every message in the game. WARNING: SLOW!";
	}
}

bool ListenAllMessages::HandleMessage(uint32_t messageID, void* message)
{
	App::ConsolePrintF("Message Sent: %x", messageID);
	return false;
}