#include "stdafx.h"
#include "CLG_CellController.h"
#include "capabilitychecker.h"



CLG_CellController::CLG_CellController()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
	App::AddUpdateFunction(this);
}


CLG_CellController::~CLG_CellController()
{
}

// For internal use, do not modify.
int CLG_CellController::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CLG_CellController::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* CLG_CellController::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(CLG_CellController);
	return nullptr;
}
//-----------------------------------------------------------------------------------

void CLG_CellController::Update() {
	if (!IsCellGame()) { return; }

	UpdatePlayerHealth();

	// Stealth
	if (mbStateStealthed) {
		Simulator::cObjectPoolIndex cellIndices[200];
		auto avatar = Simulator::Cell::GetPlayerCell();
		if (!avatar) { mbStateStealthed = false; return; }

		int numCells = Simulator::Cell::FindCellsInRadius(
			CellGame.mpCellQuery,
			avatar->GetPosition(), 20.0f,  // center and radius of search
			cellIndices, 200);

		// stop all found cells from chasing the player
		for (int i = 0; i < numCells; i++)
		{
			auto cell = CellGame.mCells.Get(cellIndices[i]);
			if (cell->IsCreature()) {
				if (cell->mChaseCellIndex == avatar->Index()) {
					cell->mChaseCellIndex = 0;
					cell->mChaseCellTime = 0.7f;
					cell->mIsIdle = true;
				}
				if (cell->mFleeCellIndex == avatar->Index()) {
					cell->mFleeCellIndex = 0;
					cell->mFleeCellTime = 0.4f;
					cell->mIsIdle = true;
				}
			}
		}
	}
}

bool modulateplayerhealth = true;
// TODO: eventually do this for more cells?
void CLG_CellController::UpdatePlayerHealth() {
	auto avatar = Simulator::Cell::GetPlayerCell();
	mPrevPlayerHealth = mCurrentPlayerHealth;
	mCurrentPlayerHealth = avatar->mHealthPoints;

	// player respawned
	if (mPrevPlayerHealth == 0 && mCurrentPlayerHealth == 6) {

	}
	else if (mPrevPlayerHealth != mCurrentPlayerHealth) {
		// player has taken damage
		if (mPrevPlayerHealth < mCurrentPlayerHealth) {
			// disable stealth
			if (mbStateStealthed) {
				ToggleStealth(false);
			}
		}
		if (modulateplayerhealth) {
			modulateplayerhealth = false;
			int damagetaken = mPrevPlayerHealth - mCurrentPlayerHealth;
			float healthbuff = 2.0f;
			float newdamagetaken = damagetaken / healthbuff;
			if (newdamagetaken < 0) {
				newdamagetaken = (float)clamp(int(floor(newdamagetaken)), -12, -1);
			}
			else {
				newdamagetaken = (float)clamp(int(ceil(newdamagetaken)), 1, 12);
			}
			avatar->mHealthPoints = int(mPrevPlayerHealth - newdamagetaken);
		}
		else {
			modulateplayerhealth = true;
		}
	}
	
		
	// TODO: also do this if the player deals damage to an NPC cell via spikes or electric
}

//-----------------------------------------------------------------------------------

int CLG_CellController::GetJetLvl() const {
	if (!IsCellGame()) { return 0;  }
	auto avatar = Simulator::Cell::GetPlayerCell();
	if (avatar) {
		auto model = avatar->mModelKey;
		return CapabilityChecker.GetCapabilityLevelFromResource(model, 0x11B79A72); //modelCapabilityCellJet
	}
	return 0;
}

// TODO: Give active abilities cooldowns and interface

void CLG_CellController::JetBurst() {
	if (!IsCellGame()) { return; }
	auto avatar = Simulator::Cell::GetPlayerCell();
	// TODO: make this always
	// why did i trail off when making this comment last year, now i have no idea what this meant
	avatar->field_90 *= -50.0f;
	Simulator::Cell::PlayAnimation(avatar, avatar, CellAnimations::kAnimIndexCellMoveJetBlink, avatar->mCurrentAnimation);
}

void CLG_CellController::ToggleStealth(bool active) {
	if (!IsCellGame()) { return; }
	auto avatar = Simulator::Cell::GetPlayerCell();
	mbStateStealthed = active;
	if (active) {
		avatar->mTargetOpacity = 0.4f;
		// TODO: have a loop that makes cells forget about the player that is active while this var is true.
		// have the player unstealth on taking damage or hitting another cell. do not unstealth on poison kills.
	}
	else {
		avatar->mTargetOpacity = 1.05f;
	}
	
}

void CLG_CellController::Pheromone(bool attract, float radius, float time) {
	if (!IsCellGame()) { return; }
	auto avatar = Simulator::Cell::GetPlayerCell();

	Simulator::Cell::PlayAnimation(avatar, avatar, CellAnimations::kAnimIndexEpicCellProboscised, CellAnimations::kAnimIndexEpicCellProboscised);

	Simulator::cObjectPoolIndex cellIndices[200];
	int numCells = Simulator::Cell::FindCellsInRadius(
		CellGame.mpCellQuery,
		avatar->GetPosition(), radius,  // center and radius of search
		cellIndices, 200);

	if (attract) {
		for (int i = 0; i < numCells; i++)
		{
			auto cell = CellGame.mCells.Get(cellIndices[i]);
			if (cell->IsCreature()) {
				cell->mChaseCellTime = radius;
				cell->mChaseCellIndex = avatar->Index();
				// If we don't disable this, fleeing cells will not chase us
				cell->mFleeCellTime = 0.0f;
				cell->mFleeCellIndex = 0;
				Simulator::Cell::PlayAnimation(cell, cell, CellAnimations::kAnimIndexEpicCellProboscised, CellAnimations::kAnimIndexEpicCellProboscised);
			}
		}
	}
	else {
		for (int i = 0; i < numCells; i++)
		{
			auto cell = CellGame.mCells.Get(cellIndices[i]);
			if (cell->IsCreature()) {
				cell->mChaseCellTime = 0.0f;
				cell->mChaseCellIndex = 0; 
				// If we don't disable this, chasing cells will not flee us
				cell->mFleeCellTime = radius;
				cell->mFleeCellIndex = avatar->Index();

			}
		}
	}
	
}

//-----------------------------------------------------------------------------------

int CLG_CellController::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool CLG_CellController::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsCellGame()) { return false; }
	if (!Simulator::Cell::GetPlayerCell() || Simulator::Cell::GetPlayerCell()->mHealthPoints == 0) {
		return false;
	}

	// pressed enter or escape or etc
	if (message.IsType(kMsgKeyDown)) {
		if (message.Key.vkey == VK_SPACE) {
			if (true || GetJetLvl() > 0) {
				ToggleStealth(!mbStateStealthed);
			}
			return false;
		}
	}

	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}
