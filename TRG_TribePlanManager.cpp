#include "stdafx.h"
#include "TRG_TribePlanManager.h"

TRG_TribePlanManager::TRG_TribePlanManager()
{
	App::AddUpdateFunction(this);
	WindowManager.GetMainWindow()->AddWinProc(this);
	CursorManager.Load(0xFA09CD25, u"cursor_construct");
}


TRG_TribePlanManager::~TRG_TribePlanManager()
{
}

// For internal use, do not modify.
int TRG_TribePlanManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int TRG_TribePlanManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* TRG_TribePlanManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(TRG_TribePlanManager);
	return nullptr;
}
//--------------------------------------------------------------------------------------

void TRG_TribePlanManager::Update()
{
	if (IsTribeGame()) {

		UpdateToolsAmount();
		UpdateToolsProgress();
		UpdatePopulation();

		// Only run when the rollover UI is visible
		auto window = WindowManager.GetMainWindow();
		auto textfield = window->FindWindowByID(0x0199A7D1);
		if (textfield) {
			UpdateTribePopulationUI();
		}
	}
}

//-----------------------------------------
// POPULATION

void TRG_TribePlanManager::UpdatePopulation()
{
	auto tribe = GameNounManager.GetPlayerTribe();
	// If amount of members has changed, run this func
	if (tribe->mTribeMembers.size() > mPopulation) {
		if (tribe->mSelectableMembers.size() == mPopulationSelectable) {
			AddedBaby();
		}
		AddedMember();
	}
	mPopulation = tribe->mTribeMembers.size();
	mPopulationSelectable = tribe->mSelectableMembers.size();
}

void TRG_TribePlanManager::AddedBaby() {
	mbBabySpawning = false;
}

void TRG_TribePlanManager::AddedMember() {
}

void TRG_TribePlanManager::UpdateTribePopulationUI() {
	auto window = WindowManager.GetMainWindow();
	auto textfield = window->FindWindowByID(0x0199A7D1); // ID of the NEW population text in Rollover_TribeHut.spui and Rollover_Tribe_Minimap.spui
	if (textfield) {

		cTribePtr tribe;

		//-------------------------------
		// INITIAL - SEARCH BY ROLLOVER
		//-------------------------------
		// loop thru all tribes to check hut rollover status
		for (auto item : Simulator::GetData<Simulator::cTribe>()) {
			if (item->mpHut->IsRolledOver()) {
				tribe = item;
				break;
			}
		}
		//-------------------------------
		// BACKUP - SEARCH BY NAME
		//-------------------------------
		if (!tribe) {
		// use the name to determine what village we are hovering over (tried color, didnt work :/ )
			auto nameText = window->FindWindowByID(0x031e94d0);
			auto textCaption = nameText->GetCaption();
			// if the text is "Noogie", this means it is invalid and should cancel.
			auto compare1 = string16(u"Noogie").compare(textCaption);
			if (compare1 == 0) { return; }

			// loop thru all tribes to compare names
			for (auto item : Simulator::GetData<Simulator::cTribe>()) {
				auto tribeName = item->GetCommunityName();
				auto compare = tribeName.compare(textCaption);
				if (compare == 0) {
					tribe = item;
					break;
				}
			}
		}
		// Found a tribe, get its population
		if (tribe) {
			int currentPop = tribe->GetTribeMembers().size();
			int maxPop = GetTribeMaxPopulation(tribe);

			// create the population caption string
			eastl::string16 captionStr;
			captionStr.sprintf(u"%i / %i", currentPop, maxPop);

			// assign the caption string
			textfield->SetCaption(captionStr.c_str());
			textfield->SetVisible(true);

			UpdateTribeBabyUI(tribe, currentPop, maxPop);
			
		}
	}
}

void TRG_TribePlanManager::UpdateTribeBabyUI(cTribePtr tribe, int population, int maxpopulation) {
	int currentPop = population;
	int maxPop = maxpopulation;
	if (population == -1 || maxpopulation == -1) {
		currentPop = tribe->GetTribeMembers().size();
		maxPop = GetTribeMaxPopulation(tribe);
	}
	
	// find baby button window
	auto window = WindowManager.GetMainWindow();
	auto babyButtonWinParent = window->FindWindowByID(0x0625CD5F);
	if (!babyButtonWinParent) { return; }
	auto babyButtonWin = babyButtonWinParent->FindWindowByID(0x0625CD52);

	if (babyButtonWin) {
		if (currentPop >= maxPop || mbBabySpawning) {
			babyButtonWin->SetEnabled(false);
		}
		else {
			babyButtonWin->SetEnabled(true);
		}
	}
}

// TODO: this!
int TRG_TribePlanManager::GetTribeMaxPopulation(cTribePtr tribe) const {
	int population = basePopulation;
	// count all the houses
	for (auto item : tribe->GetTools()) {
		auto toolType = item->GetToolType();
		if (toolType == 11) {
			population += populationPerHouse;
		}
	}
	return population;
}



//-----------------------------------------
// TOOLDATA

cTribeToolData* TRG_TribePlanManager::TribeToolDataFromProp(ResourceKey key) {

}

//-----------------------------------------
// TOOLS

void TRG_TribePlanManager::UpdateToolsAmount()
{
	if (!IsTribeGame()) { return; }

	auto tools = Simulator::GetData<Simulator::cTribeTool>();
	// If amount of tools has changed, run this func
	if (tools.size() > mToolsAmount && !IsToolFirePit(tools[tools.size() - 1])) {
		AddedTool();
	}
	mToolsAmount = tools.size();
}

//cTribeToolPtr hovered;
void TRG_TribePlanManager::UpdateToolsProgress() {
	// remove old
	for (size_t i = 0; i < mpInProgressTools.size(); i++) {
		auto tool = mpInProgressTools[i];
		if (tool) {
			//if (tool->IsRolledOver()) { hovered = tool; }
			// health is at max, remove from this list
			if (tool->mHealthPoints > tool->mMaxHealthPoints - 0.5) {
				ToolComplete(i);
				mpInProgressTools[i] = nullptr;
				mpPlotHealths[i] = -1;

				IVisualEffectPtr effectPoof;
				if (EffectsManager.CreateVisualEffect(id("city_building_plop_dust"), 0, effectPoof)) {
					Transform trans = Transform();
					Transform().SetOffset(tool->GetPosition());
					Transform().SetRotation(tool->GetOrientation());
					effectPoof->SetSourceTransform(trans);
					effectPoof->Start();
					// todo: not working
				}
				
				// check if the tool's health is the same as the one in the ui, and if so, close the UI.
				// TODO: this.
				// OR, maybe check if the hovered hut is the same one as this..?
				if (GetHoveredConstructionPlot() == tool) {
					auto popup = WindowManager.GetMainWindow()->FindWindowByID(0x93310E4A);
					popup->SetVisible(false);
				}
				//auto popup = WindowManager.GetMainWindow()->FindWindowByID(0x93310E4A);
				//if (popup) {
				//	auto textwindow = popup->FindWindowByID(0x04D039A8);
				//	if (textwindow->GetCaption() == tool->)

			}
			else if (tool->mHealthPoints > (0.825 * tool->mMaxHealthPoints) && tool->mHealthPoints < tool->mMaxHealthPoints) {
				tool->SetModelKey(GetPlotModel(tool));
			}
		}
		
	}
	return;

}
/*
// raycast from camera to the UI pos, then find nearest construction plot to that point.
cTribeToolPtr TRG_TribePlanManager::GetConstructionPlotFromScreenPos(Vector2 pos) const {
	auto pViewer = CameraManager.GetViewer();

	Vector3 camPos = Vector3(0, 0, 0);
	Vector3 camDir = Vector3(0, 0, 0);

	// Get vector to that point on the screen
	auto windowArea = WindowManager.GetMainWindow()->GetArea();
	pViewer->GetCameraToPoint(pos.x, pos.y, camPos, camDir);

	// raycast to ground using the vecs
	if (PlanetModel.mpTerrain) {
		auto terrainPos = PlanetModel.mpTerrain->Raycast(camPos, camDir);

		float last_distance = 512;
		cTribeToolPtr closest_plot = nullptr;

		for (auto plot : mpInProgressTools) {
			if ()
			float dist = Math::distance(plot->GetPosition(), terrainPos);
			if (dist < last_distance) {
				closest_plot = plot;
			}
		}
		return closest_plot;
	}
	
	return nullptr;
}*/

void TRG_TribePlanManager::AddedTool() {
	auto tools = Simulator::GetData<Simulator::cTribeTool>();
	if (tools.size() > 0) {
		auto tool = tools[tools.size() - 1];
		tool->SetHealthPoints(tool->GetMaxHitPoints() * progress_value);
		tool->SetModelKey(plot_social01);

		// do this last
		mpInProgressTools.push_back(tool);
		mpPlotHealths.push_back(tool->mHealthPoints);
	}
}

void TRG_TribePlanManager::ToolComplete(int index) {

}



bool TRG_TribePlanManager::IsToolFirePit(cTribeToolPtr tool) const {
	if (!tool) { return false; }
	switch (tool->GetModelKey().instanceID) {
		case 0x1AE006AB:
		case 0x1AE006A8:
		case 0x1AE006A9:
			return true;
			break;
	}
	return false;
}

// get model from tool type and health level
ResourceKey TRG_TribePlanManager::GetPlotModel(cTribeToolPtr tool) const {
	bool modelidx = tool->mHealthPoints > (0.825 * tool->mMaxHealthPoints);

	if (modelidx) { return plot_social02; }
	else { return plot_social01; }

	return ResourceKey();
}

cTribeToolPtr TRG_TribePlanManager::GetHoveredConstructionPlot() const {
	for (auto tool : mpInProgressTools) {
		if (tool && tool->IsRolledOver()) {
			return tool;
		}
	}
	return nullptr;
}

bool TRG_TribePlanManager::IsConstructionPlotHovered() const {
	for (auto tool : mpInProgressTools) {
		if (tool && tool->IsRolledOver()) {
			return true;
		}
	}
	return false;
}

bool TRG_TribePlanManager::IsToolInProgress(cTribeToolPtr tool) const {
	if (!tool) { return false; }
	for (auto item : mpInProgressTools) {
		if (tool == item) {
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------

int TRG_TribePlanManager::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool TRG_TribePlanManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsTribeGame()) { return false; }

	if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();
		// pressed baby button
		if (id == 0x0625CD52) {
			mbBabySpawning = true;
			//debug
			auto tribe = GameNounManager.GetPlayerTribe();
			//tribe->func9Ch(0, true);
			tribe->funcB0h();
		}
	}
	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}
