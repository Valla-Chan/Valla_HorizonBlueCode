#include "stdafx.h"
#include "SendMessage.h"

SendMessageCheat::SendMessageCheat()
{
}


SendMessageCheat::~SendMessageCheat()
{
}


void SendMessageCheat::ParseLine(const ArgScript::Line& line)
{
	mParameter = line.GetArguments(1)[0];
	uint32_t message = mpFormatParser->ParseUInt(mParameter);
	MessageManager.MessageSend(message, nullptr);

}

const char* SendMessageCheat::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sends a game message by ID.";
	}
	else {
		return "SendMessage: Sends a game message by ID.";
	}
}
