#include "stdafx.h"
#include "PrintCursor.h"

PrintCursor::PrintCursor()
{
}


PrintCursor::~PrintCursor()
{
}


void PrintCursor::ParseLine(const ArgScript::Line& line)
{
	App::ConsolePrintF("Current Cursor ID: 0x%x", CursorManager.GetActiveCursor());
}

const char* PrintCursor::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Prints the ID of the current cursor to the console.";
	}
	else {
		return "PrintCursor: Prints the ID of the current cursor to the console.";
	}
}
