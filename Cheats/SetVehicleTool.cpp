#include "stdafx.h"
#include "SetVehicleTool.h"

SetVehicleTool::SetVehicleTool()
{
}


SetVehicleTool::~SetVehicleTool()
{
}

using namespace Simulator;
void SetVehicleTool::ParseLine(const ArgScript::Line& line)
{
	if (!(IsScenarioMode() || IsSpaceGame() || IsCivGame())) { return; }
	mParameter = line.GetArguments(1)[0];

	auto vehicles = Simulator::GetDataByCast<cVehicle>();
	for (auto vehicle : vehicles) {
		cSpaceToolDataPtr tool;
		if (ToolManager.LoadTool(ResourceKey(id(mParameter),0,0), tool)) {
			if (tool) {
				vehicle->mpWeapon = tool;
			}
		}
		//vehicle->mpWeapon =
	}
}

const char* SetVehicleTool::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Debug Cheat. Sets all vehicles to use the specified tool.";
	}
	else {
		return "SetVehicleTool: Debug Cheat. Sets all vehicles to use the specified tool.";
	}
}
