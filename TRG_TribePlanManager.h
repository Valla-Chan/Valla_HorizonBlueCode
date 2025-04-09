#pragma once

#include <Spore\BasicIncludes.h>
#include "TRG_ToolIDs.h"
#include "TribeToolStratManager.h"
//#include "TribeToolDataKeys.h"

#define TRG_TribePlanManagerPtr intrusive_ptr<TRG_TribePlanManager>

using namespace UTFWin;
using namespace Simulator;
using namespace UI;
using namespace Editors;

class TRG_TribePlanManager 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	//, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("TRG_TribePlanManager");
	
	TRG_TribePlanManager();
	~TRG_TribePlanManager();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;
	
	// GENERAL
	//---------------------------------------


	// POPULATION
	//---------------------------------------
	const int basePopulation = 3;
	const int populationPerHouse = 1;

	int mPopulation = 0;
	int mPopulationSelectable = 0;
	bool mbBabySpawning = false;

	bool TribeHasToolType(cTribePtr tribe, int tooltype) const;

	void UpdatePopulation();
	void AddedBaby();
	void AddedMember();
	void UpdateTribePopulationUI();
	void UpdateTribeBabyUI(cTribePtr tribe, int population = -1, int maxpopulation = -1);
	int GetTribeMaxPopulation(cTribePtr tribe) const;

	// TOOLS
	//---------------------------------------

	const float progress_value = 0.67f; // starter health scale for hut construction

	// TODO: a lot of this stuff needs to be moved to the tribetoolstratmanager, and removed from here.

	// additional data for new tribe tools

	struct ToolMetadata {
		int mHandHeldIndex = 0;
		uint32_t mHandHeldItemEffect = 0x0;
		uint32_t mToolEnRouteAnim = 0x0;

		bool mbToolPrebuilt = false; // todo: make this work like the houses except in its own range of decor items that is functionally infinite, and map the used ID number to its palette tool ID
		bool mbToolAllowMultiple = false;
		bool mbToolDisableRollover = false;

		ResourceKey mSpecializedName = {};
		ResourceKey mAbilityKey = {};
	};

	hash_map<int, uint32_t> tribeToolIDs;
	hash_map<int, cTribeToolStratManager::ToolMetadata*> tribeToolMetadata;

	//---------------------------------------

	int mToolsAmount = 0;
	int mPurchasedTools = 0;
	bool mbHutsPrebuilt = false;
	vector<cTribeToolPtr> mpInProgressTools = {};
	vector<float> mpPlotHealths = {};

	vector<Vector2> mToolQueueCitizenData; // ToolID, HandheldID


	//---------------------------------------

	eastl::array<cTribeToolData*, 64> mToolDataArray; // hb tribal tool data

	void PopulateTribeToolData();
	ToolMetadata* TRG_TribePlanManager::GetTribeToolMetadata(int toolType) const;
	cTribeToolData* GetTribeToolData(int toolType) const;
	static cTribeToolData* TribeToolDataFromProp(ResourceKey key, int typeIDoverride = -1);
	static int TribeToolTypeIDFromProp(ResourceKey key);
	static ToolMetadata* TribeToolMetadataFromProp(ResourceKey key);
	void SetDesiredHandheldItem(cCreatureCitizenPtr citizen);
	
	ResourceKey GetCitizenNameLocaleResource(cCreatureCitizenPtr citizen) const;
	// Add a chief creature's diet value to the queue
	void AddCitizenToolTypeToQueue(int toolID, int handheldID);
	void RemoveCitizenToolTypeQueue();
	//int NextQueueItem();
	uint32_t ConvertToolEffectID(uint32_t instanceId);

	//---------------------------------------

	void UpdateToolsProgress(); // check if under construction tool has been completed

	void AddedTool(cTribeToolPtr tool, cTribePtr tribe); // new tool has just been added
	void ToolComplete(cTribeToolPtr tool); // tool has finished construction

	//cTribeToolPtr GetConstructionPlotFromScreenPos(Vector2 pos) const;
	bool IsToolFirePit(cTribeToolPtr tool) const;
	bool IsToolHome(cTribeToolPtr tool) const;
	int GetNewHomeID(cTribePtr tribe) const;
	ResourceKey GetPlotModel(cTribeToolPtr tool) const;
	void RemoveFromProgress(cTribeToolPtr tool);
	cTribeToolPtr GetHoveredConstructionPlot() const;
	bool IsConstructionPlotHovered() const;
	bool IsToolInProgress(cTribeToolPtr tool) const;

	//Vector2 GetDecorIDRange(int toolType) const;
	//int GetDecorRealToolID(int toolType) const; // get the actual tool ID from the modified one
	//int GetNewDecorID(int toolType, cTribePtr tribe) const;


	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	//bool HandleMessage(uint32_t messageID, void* msg) override;
	void ResetBabySpawningVar();
private:
	intrusive_ptr<Simulator::ScheduledTaskListener> mBabySpawnTask;
};
