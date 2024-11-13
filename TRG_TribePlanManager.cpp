#include "stdafx.h"
#include "TRG_TribePlanManager.h"
#include <Spore\Editors\Editor.h>

TRG_TribePlanManager::TRG_TribePlanManager()
{
	App::AddUpdateFunction(this);
	WindowManager.GetMainWindow()->AddWinProc(this);
	CursorManager.Load(0xFA09CD25, u"cursor_construct");
	//MessageManager.AddListener(this, id("PlannerPopup"));
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

		UpdateToolsProgress();
		UpdatePopulation();

		// Only run when the rollover UI is visible
		auto window = WindowManager.GetMainWindow();
		auto textfield = window->FindWindowByID(0x0199A7D1);
		if (textfield) {
			UpdateTribePopulationUI();
		}

		//// Handle manual hovered rollover
		//// This must go last
		auto hovered = GameViewManager.GetHoveredObject();
		if (hovered) {
			// Tool
			auto tribetool = object_cast<cTribeTool>(hovered);
			// Only display info for tools from our tribe.
			if (tribetool && tribetool->mTribe == GameNounManager.GetPlayerTribe()) {
				SimulatorRollover::ShowRollover(tribetool);
				return;
			}
		}
		
	}
}

//-----------------------------------------
// GENERAL



//-----------------------------------------
// POPULATION

void TRG_TribePlanManager::UpdatePopulation()
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

// Add the base population and the house count
int TRG_TribePlanManager::GetTribeMaxPopulation(cTribePtr tribe) const {
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



//-----------------------------------------
// TOOLDATA

// TODO: Maybe read and store the files from the folder in a vector, like the normal code does.
cTribeToolData* TRG_TribePlanManager::GetTribeToolData(int toolType) const {
	using namespace TypeIDs;
	if (toolType > TRG_TribePlanManager::ToolTypes::HomePalette) {
		//instantiate
		ResourceKey toolKey = ResourceKey(0x0, Names::prop, GroupIDs::TribeToolData);
		int typeIDoverride = -1;

		// Home hut
		// Note: The entire range of Housing objects all return the data from tool 11
		if (toolType > TRG_TribePlanManager::ToolTypes::HomePalette && toolType <= TRG_TribePlanManager::ToolTypes::HomeEnd) {
			toolKey.instanceID = id("homehut");
			typeIDoverride = toolType;
		}
		// Island Event Rare
		else if (toolType == EventRare) {
			// TODO: this should maybe do something special that returns the actual model of the rare as well?
			toolKey.instanceID = id("homehut");
			typeIDoverride = toolType;
		}
		// all else is WatchTower for now.
		else {
			toolKey.instanceID = id("WatchTower");
		}

		// get the actual data from this file
		return TribeToolDataFromProp(toolKey, typeIDoverride);

	}
	return nullptr;
}

cTribeToolData* TRG_TribePlanManager::TribeToolDataFromProp(ResourceKey key, int typeIDoverride) {
	cTribeToolData* toolData = new(cTribeToolData);
	toolData->mFileKey = key;
	toolData->mMaxNumSlots = 999;

	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(key.instanceID, key.groupID, mpPropList))
	{
		App::Property::GetInt32(mpPropList.get(), 0x04294750, toolData->mToolType);
		App::Property::GetInt32(mpPropList.get(), 0x04294751, toolData->mToolClass);
		App::Property::GetInt32(mpPropList.get(), 0x02166464, toolData->mPrice);
		App::Property::GetFloat(mpPropList.get(), 0x03A26420, toolData->mToolHealth);

		App::Property::GetKey(mpPropList.get(), 0x04294752, toolData->mRackModelKey);
		App::Property::GetKey(mpPropList.get(), 0x492D3895, toolData->mToolDamageHiKey);
		App::Property::GetKey(mpPropList.get(), 0x492D3896, toolData->mToolDamageMdKey);
		App::Property::GetKey(mpPropList.get(), 0x492D3897, toolData->mToolDamageLoKey);

		// Ability
		ResourceKey abilityKey;
		cCreatureAbility* ability = new(cCreatureAbility);
		PropertyListPtr mpAbilityPropList;
		App::Property::GetKey(mpPropList.get(), 0x051CDDBC, abilityKey);
		if (PropManager.GetPropertyList(abilityKey.instanceID, abilityKey.groupID, mpAbilityPropList))
		{
			cCreatureAbility::Parse(ability, mpAbilityPropList.get());
			if (ability) {
				toolData->mToolAbility = ability;
			}
		}
		
		App::Property::GetUInt32(mpPropList.get(), 0x04294753, toolData->mToolIconID);
		App::Property::GetUInt32(mpPropList.get(), 0x04294754, toolData->mDefaultToolIdleAnim);
		App::Property::GetUInt32(mpPropList.get(), 0x04294755, toolData->mLookaroundToolIdleAnim);
		App::Property::GetUInt32(mpPropList.get(), 0x05F09049, toolData->mGrasperOverlay);
		App::Property::GetUInt32(mpPropList.get(), 0x062ECADE, toolData->mEnRouteAnim);

		App::Property::GetText(mpPropList.get(), 0x8F6FC401, toolData->mName);
	}
	//toolData->mToolType = 4;
	if (typeIDoverride != -1) {
		toolData->mToolType = typeIDoverride;
	}
	toolData->mToolClass = 1;

	return toolData;
}

//-----------------------------------------
// TOOLS


//cTribeToolPtr hovered;
void TRG_TribePlanManager::UpdateToolsProgress() {
	// remove old
	for (size_t i = 0; i < mpInProgressTools.size(); i++) {
		auto tool = mpInProgressTools[i];
		if (tool) {
			//if (tool->IsRolledOver()) { hovered = tool; }
			// health is at max, remove from this list
			if (tool->mHealthPoints > tool->mMaxHealthPoints - 0.5) {
				ToolComplete(tool);
				mpInProgressTools[i] = nullptr;
				mpPlotHealths[i] = -1;

				IVisualEffectPtr effectPoof;
				if (EffectsManager.CreateVisualEffect(id("city_building_plop_dust"), 0, effectPoof)) {
					Transform trans = Transform();
					Transform().SetOffset(tool->GetPosition());
					Transform().SetRotation(tool->GetOrientation());
					Transform().SetScale(1.0f);
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

void TRG_TribePlanManager::AddedTool(cTribeToolPtr tool, cTribePtr tribe) {
	int toolType = tool->GetToolType();
	// Campfires should not count
	if (toolType == 10) {
		return;
	}

	// Make tribal huts have to be built
	if (!mbHutsPrebuilt) {
		tool->SetHealthPoints(tool->GetMaxHitPoints() * progress_value);
		tool->SetModelKey(plot_social01);

		// do this last
		mpInProgressTools.push_back(tool);
		mpPlotHealths.push_back(tool->mHealthPoints);
	}

	//---------------------------------------------

	// TODO: Allow home type huts to be added until they hit the upper limit for house count, based on tribe tier
	// when the last possible one is spawned, we will have to do some other fancy stuff to dynamically swap IDs on existing homes
	// as you buy and sell them to and from the current limit
	// and also as the tribe tier changes
	// OR
	// Maybe consider just showing or hiding a UI window that grays out the housing slot and says something like
	// (6/6) you need a higher tier tribe for more homes

	if (toolType == HomePalette) {
		tool->SetToolType(GetNewHomeID(tribe));
	}

}

void TRG_TribePlanManager::ToolComplete(cTribeToolPtr tool) {
	if (IsToolHome(tool)) {
		//MessageManager.MessageSend(id("UpdateHomes"), nullptr);
	}
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

bool TRG_TribePlanManager::IsToolHome(cTribeToolPtr tool) const {
	auto toolType = tool->GetToolType();
	return (toolType >= HomeStart && toolType <= HomeEnd);
}


// Get the nearest open slot for 'Home' type huts, from HomeStart to HomeEnd
int TRG_TribePlanManager::GetNewHomeID(cTribePtr tribe) const {
	for (int i = HomeStart; i <= HomeEnd; i++) {
		if (!tribe->GetToolByType(i)) {
			return i;
		}

	}
	return 12;
}

// get model from tool type and health level
ResourceKey TRG_TribePlanManager::GetPlotModel(cTribeToolPtr tool) const {
	bool modelidx = tool->mHealthPoints > (0.825 * tool->mMaxHealthPoints);

	if (modelidx) { return plot_social02; }
	else { return plot_social01; }

	return tool->GetModelKey();
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
		}

	}
	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}

/*
bool TRG_TribePlanManager::HandleMessage(uint32_t messageID, void* msg) {
	if (messageID == id("PlannerPopup")) {
		auto editor = GetEditor();
		if (editor) {
			//GetEditor()->mpPartsPaletteUI->SetActiveCategory(3);
			auto palette = editor->mpPartsPaletteUI;
			editor->mpPartsPaletteUI->mpActiveCategory = editor->mpPartsPaletteUI->mCategories[2];
			//GetEditor()->mpPartsPaletteUI->mpActiveCategory = 
		}
		
	}
	return false;
}*/