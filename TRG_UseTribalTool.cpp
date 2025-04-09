#include "stdafx.h"
#include "TRG_UseTribalTool.h"

TRG_UseTribalTool::TRG_UseTribalTool()
{
}


TRG_UseTribalTool::~TRG_UseTribalTool()
{
}


void TRG_UseTribalTool::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsTribeGame()) {

		auto hoveredTool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());
		if (!hoveredTool) { return; }

		auto tribe = GameNounManager.GetPlayerTribe();
		eastl::vector<cSpatialObjectPtr>& members = tribe->GetSelectableMembers();
		for (auto member : members) {
			auto citizen = object_cast<Simulator::cCreatureCitizen>(member);
			if (member->IsSelected() && citizen) {
				citizen->DoAction(Simulator::kCitizenActionGrabTool, hoveredTool);
			}
		}
	}
}

/*
void TRG_UseTribalTool::CitizenUseTool(const ArgScript::Line& line)
{
}*/

const char* TRG_UseTribalTool::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Forces selected creatures to interact with a tool shed.";
	}
	else {
		return "UseTribeTool: Forces selected creatures to interact with a tool shed.";
	}
}
