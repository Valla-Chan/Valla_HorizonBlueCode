#pragma once

#include <Spore\BasicIncludes.h>
#include "TribeToolStratManager.h"
#include "TribeToolManager.h"

#include "TRG_SuppressScavenger.h"
#include "TRG_MemberManager.h"
#include "TRG_TribeHutManager.h"
#include "TRG_TribeSlotManager.h"
#include "TRG_IslandEventManager.h"
#include "TRG_FireDanceManager.h"

#define cTribePlanManagerPtr intrusive_ptr<cTribePlanManager>
//#define TribePlanManager (*cTribePlanManager::Get())

using namespace UTFWin;
using namespace Simulator;
using namespace UI;
using namespace Editors;

class cTribePlanManager 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	//, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("cTribePlanManager");
	
	cTribePlanManager();
	~cTribePlanManager();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	static cTribePlanManager* Get();
	void Update() override;
	
	// GENERAL
	//---------------------------------------


	// SUBMANAGERS
	//---------------------------------------


	// POPULATION
	//---------------------------------------
	const int basePopulation = 3;
	const int populationPerHouse = 1;

	int mPopulation = 0;
	int mPopulationSelectable = 0; // used to check if a baby was added. this might be able to be done via detouring the add
	bool mbBabySpawning = false;

	void UpdatePopulation();
	void AddedBaby();
	void AddedMember(); // currently a stub
	int GetTribeMaxPopulation(cTribePtr tribe) const;

	//---------------------------------------

	// leave these funcs in, they are now references to the tool manager funcs.
	static bool IsToolHome(cTribeToolPtr tool);
	static bool IsToolInProgress(cTribeToolPtr tool);
	static bool TribeHasWell(cTribePtr tribe);
	static bool TribeHasScarecrow(cTribePtr tribe);

	int GetEventFlags() const override;
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

protected:
	static cTribePlanManager* sInstance;

private:
	void UpdateTribeBabyUI(cTribePtr tribe, int population = -1, int maxpopulation = -1) const;
	void ResetBabySpawningVar();
	intrusive_ptr<Simulator::ScheduledTaskListener> mBabySpawnTask;
};


