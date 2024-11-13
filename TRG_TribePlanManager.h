#pragma once

#include <Spore\BasicIncludes.h>
#include "TRG_ToolIDs.h"

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

	void UpdatePopulation();
	void AddedBaby();
	void AddedMember();
	void UpdateTribePopulationUI();
	void UpdateTribeBabyUI(cTribePtr tribe, int population = -1, int maxpopulation = -1);
	int GetTribeMaxPopulation(cTribePtr tribe) const;

	// TOOLS
	//---------------------------------------

	const float progress_value = 0.67f; // starter health scale for hut construction

	/// For tool types past those in Simulator::TribeToolType
	enum ToolTypes {
		HomePalette = 11, // only used in the palette
		// Allocate 10 slots for tribal homes
		HomeStart = 12,
		HomeEnd = 22,
		//
		EventRare = 23,
		Watchtower = 24,
	};
	

	const ResourceKey plot_social01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
	const ResourceKey plot_social02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));

	const ResourceKey plot_attack01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
	const ResourceKey plot_attack02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));

	const ResourceKey plot_heal01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
	const ResourceKey plot_heal02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));

	const ResourceKey plot_food01 = ResourceKey(id("tt_construction_social_01"), TypeIDs::Names::prop, id("toolplots"));
	const ResourceKey plot_food02 = ResourceKey(id("tt_construction_social_02"), TypeIDs::Names::prop, id("toolplots"));
	//---------------------------------------

	int mToolsAmount = 0;
	int mPurchasedTools = 0;
	bool mbHutsPrebuilt = false;
	vector<cTribeToolPtr> mpInProgressTools = {};
	vector<float> mpPlotHealths = {};

	//---------------------------------------

	eastl::array<cTribeToolData*, 64> mToolDataArray; // tool data after 11

	cTribeToolData* GetTribeToolData(int toolType) const;
	static cTribeToolData* TribeToolDataFromProp(ResourceKey key, int typeIDoverride = -1);

	//---------------------------------------

	void UpdateToolsProgress(); // check if under construction tool has been completed

	void AddedTool(cTribeToolPtr tool, cTribePtr tribe); // new tool has just been added
	void ToolComplete(cTribeToolPtr tool); // tool has finished construction

	//cTribeToolPtr GetConstructionPlotFromScreenPos(Vector2 pos) const;
	bool IsToolFirePit(cTribeToolPtr tool) const;
	bool IsToolHome(cTribeToolPtr tool) const;
	int GetNewHomeID(cTribePtr tribe) const;
	ResourceKey GetPlotModel(cTribeToolPtr tool) const;
	cTribeToolPtr GetHoveredConstructionPlot() const;
	bool IsConstructionPlotHovered() const;
	bool IsToolInProgress(cTribeToolPtr tool) const;

	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	//bool HandleMessage(uint32_t messageID, void* msg) override;

};
