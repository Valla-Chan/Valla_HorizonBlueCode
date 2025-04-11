#include "stdafx.h"
#include "TribePlanManager.h"
#include <Spore\Editors\Editor.h>

cTribePlanManager::cTribePlanManager()
{
	sInstance = this;
	trg_suppressscavenger = new(TRG_SuppressScavenger);
	trg_hutmanager = new(TRG_TribeHutManager);
	//TRG_TribeSlotManager* trg_slotmanager = new(TRG_TribeSlotManager);
	trg_ieventmanager = new(TRG_IslandEventManager); // TODO: make standalone?
	trg_firedancemanager = new(TRG_FireDanceManager);
	new(TRG_SuppressScavenger);

	App::AddUpdateFunction(this);
	WindowManager.GetMainWindow()->AddWinProc(this);
	CursorManager.Load(0xFA09CD25, u"cursor_construct");
}



cTribePlanManager::~cTribePlanManager()
{
	sInstance = nullptr;
	trg_suppressscavenger = nullptr;
}

cTribePlanManager* cTribePlanManager::sInstance;

cTribePlanManager* cTribePlanManager::Get()
{
	return sInstance;
}

// For internal use, do not modify.
int cTribePlanManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cTribePlanManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cTribePlanManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(cTribePlanManager);
	return nullptr;
}

//--------------------------------------------------------------------------------------

// TODO: ideally there should be a better way to do this... maybe detour the funcs that add creatures to the tribe, creature deaths, and etc? make sure to account for queueing for deletion.
// Maybe have this react to a message, also, in the case of manually adding creatures from other tribes, etc
void cTribePlanManager::Update()
{
	if (IsTribeGame()) {
		UpdatePopulation();
	}
}

//-----------------------------------------
// POPULATION

void cTribePlanManager::UpdatePopulation()
{
	auto tribe = GameNounManager.GetPlayerTribe();
	if (!tribe) { return; }
	// If amount of members has changed, run this func
	if (int(tribe->mTribeMembers.size()) > mPopulation) {
		if (tribe->mSelectableMembers.size() == mPopulationSelectable) {
			AddedBaby();
		}
		AddedMember();
	}
	mPopulation = tribe->mTribeMembers.size();
	mPopulationSelectable = tribe->mSelectableMembers.size();
}

void cTribePlanManager::AddedBaby() {
	mbBabySpawning = false;
}

void cTribePlanManager::AddedMember() {
}

// The baby UI has to be manually controlled, due to a vanilla-spore glitch
void cTribePlanManager::UpdateTribeBabyUI(cTribePtr tribe, int population, int maxpopulation) const {
	int currentPop = population;
	int maxPop = maxpopulation;
	if (population == -1 || maxpopulation == -1) {
		currentPop = tribe->GetTribeMembers().size();
		maxPop = GetTribeMaxPopulation(tribe);
	}
	
	// Note: not sure whether to use this. drop the visible population by this amount if chieftain is dead
	//if (tribe->GetLeaderCitizen()->mHealthPoints == 0) {
	//	currentPop-=1;
	//}

	// Find baby button window
	auto window = WindowManager.GetMainWindow();
	auto babyButtonWinParent = window->FindWindowByID(0x0625CD5F);
	if (!babyButtonWinParent) { return; }
	auto babyButtonWin = babyButtonWinParent->FindWindowByID(0x0625CD52);

	// If baby has been spawned but not hatched, disable it. Otherwise, enable it.
	if (babyButtonWin) {
		if (currentPop >= maxPop || mbBabySpawning) {
			babyButtonWin->SetEnabled(false);
		}
		else {
			babyButtonWin->SetEnabled(true);
		}
	}
}

// Add the base population and the house count
int cTribePlanManager::GetTribeMaxPopulation(cTribePtr tribe) const {
	int population = basePopulation;
	// count all the houses
	for (auto item : tribe->GetTools()) {
		auto toolType = item->GetToolType();
		if (IsToolHome(item) && !IsToolInProgress(item)) {
			population += populationPerHouse;
		}
	}
	return population;
}


bool cTribePlanManager::IsToolHome(cTribeToolPtr tool) {
	return TribeToolManager.IsToolHome(tool);
}

bool cTribePlanManager::IsToolInProgress(cTribeToolPtr tool) {
	return TribeToolManager.IsToolInProgress(tool);
}

bool cTribePlanManager::TribeHasWell(cTribePtr tribe) {
	bool has = TribeToolManager.TribeHasToolType(tribe, Well1);
	if (has) { return true; }
	has = TribeToolManager.TribeHasToolType(tribe, Well2);
	if (has) { return true; }
	has = TribeToolManager.TribeHasToolType(tribe, Well3);
	if (has) { return true; }
	return false;
}

bool cTribePlanManager::TribeHasScarecrow(cTribePtr tribe) {
	return TribeToolManager.TribeHasToolType(tribe, Scarecrow);
}

//--------------------------------------------------------------------------------------
// Messages

int cTribePlanManager::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

bool cTribePlanManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsTribeGame()) { return false; }

	// Button Click
	if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();
		// Pressed Baby Button
		if (id == 0x0625CD52) {
			if (mBabySpawnTask && !mBabySpawnTask->HasExecuted()) {
				Simulator::RemoveScheduledTask(mBabySpawnTask);
			}
			mbBabySpawning = true;
			mBabySpawnTask = Simulator::ScheduleTask(this, &cTribePlanManager::ResetBabySpawningVar, 6.0f);
		}

	}
	return false;
}

// make sure the baby spawning var doesnt get stuck on
void cTribePlanManager::ResetBabySpawningVar() {
	mbBabySpawning = false;
}
