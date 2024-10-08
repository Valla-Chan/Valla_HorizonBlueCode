#pragma once

#include <Spore\BasicIncludes.h>

#define TRG_TribePlanManagerPtr intrusive_ptr<TRG_TribePlanManager>

using namespace UTFWin;
using namespace Simulator;

class TRG_TribePlanManager 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("TRG_TribePlanManager");
	
	TRG_TribePlanManager();
	~TRG_TribePlanManager();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;
	
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
	const float progress_value = 0.67f;

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
	vector<cTribeToolPtr> mpInProgressTools = {};
	vector<float> mpPlotHealths = {};

	static cTribeToolData* TribeToolDataFromProp(ResourceKey key);

	void UpdateToolsAmount(); // check if tool has been added
	void UpdateToolsProgress(); // check if under construction tool has been completed

	void AddedTool(); // new tool has just been added
	void ToolComplete(int index); // tool has finished construction

	//cTribeToolPtr GetConstructionPlotFromScreenPos(Vector2 pos) const;
	bool IsToolFirePit(cTribeToolPtr tool) const;
	ResourceKey GetPlotModel(cTribeToolPtr tool) const;
	cTribeToolPtr GetHoveredConstructionPlot() const;
	bool IsConstructionPlotHovered() const;
	bool IsToolInProgress(cTribeToolPtr tool) const;


	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
};
