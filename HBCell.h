#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"
// CLG Ingame Behaviors
#include "CLG_CellController.h"

using namespace Simulator;
using namespace Simulator::Cell;

#define HBCellPtr intrusive_ptr<HBCell>

// CLG

CLG_CellController* cellcontroller;

class HBCell
{

public:

	static void Initialize() {
		cellcontroller = new(CLG_CellController);
	}

	static void Dispose() {
		cellcontroller = nullptr;
	}

	static void AttachDetours();
};

//----------
// Detours

// Whether a cell should attack another cell
static_detour(CellShouldNotAttack_detour, bool(cCellObjectData*, cCellObjectData*))
{
	bool detoured(cCellObjectData * cell1, cCellObjectData * cell2)
	{
		if (!cellcontroller->mbStateStealthed) {
			return original_function(cell1, cell2);
		}
		else if (cell2 && cell2 == Simulator::Cell::GetPlayerCell()) {
			return true;
		}
		return original_function(cell1, cell2);
	}
};

//-----------------
// ATTACH DETOURS
void HBCell::AttachDetours() {
	CellShouldNotAttack_detour::attach(GetAddress(Simulator::Cell, ShouldNotAttack));
}
