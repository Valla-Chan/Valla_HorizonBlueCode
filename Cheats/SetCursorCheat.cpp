#include "stdafx.h"
#include "SetCursorCheat.h"

SetCursorCheat::SetCursorCheat()
{
}


SetCursorCheat::~SetCursorCheat()
{
}


void SetCursorCheat::ParseLine(const ArgScript::Line& line)
{
	auto args = line.GetArguments(1);
	uint32_t id = mpFormatParser->ParseUInt(args[0]);
	bool test = CursorManager.SetActiveCursor(id);
	if (!test) {
		App::ConsolePrintF("Cursor '0x%x' does not exist.", id);
	}
}

const char* SetCursorCheat::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Sets the cursor to a valid cursor ID.";
	}
	else {
		return "SetCursor: Sets the cursor to a valid cursor ID.";
	}
}
