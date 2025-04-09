#include "stdafx.h"
#include "TRG_TribePlanManager.h"
#include <Spore\Editors\Editor.h>

TRG_TribePlanManager::TRG_TribePlanManager()
{
	App::AddUpdateFunction(this);
	WindowManager.GetMainWindow()->AddWinProc(this);
	CursorManager.Load(0xFA09CD25, u"cursor_construct");
	//MessageManager.AddListener(this, id("PlannerPopup"));
	PopulateTribeToolData();
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

bool TRG_TribePlanManager::TribeHasToolType(cTribePtr tribe, int tooltype) const {
	if (tribe && tribe->GetToolByType(tooltype)) {
		return true;
	}
	return false;
}

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

// DEPRECATED
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
			//if (maxPop > tribe->mTribeMembers.size()) {
			//	tribe->mTribeMembers.resize(maxPop);
			//	tribe->mCommunityMembers.resize(maxPop);
			//	tribe->mCommunitySize = maxPop;
			//}

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
	
	// Note: not sure whether to use this. drop the visible population by this amount if chieftain is dead
	//if (tribe->GetLeaderCitizen()->mHealthPoints == 0) {
	//	currentPop-=1;
	//}

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

// Read and store the new tool files from the folder in a vector, like the normal tool code does.
void TRG_TribePlanManager::PopulateTribeToolData() {

	eastl::vector<ResourceKey> keys;

	// pull new Tribe Tool entry props from tribe_tools~ folder
	ResourceManager.GetRecordKeyList(keys,
		&Resource::StandardFileFilter(
			ResourceKey::kWildcardID,
			GroupIDs::Names::TribeToolData,
			TypeIDs::prop,
			ResourceKey::kWildcardID
		)
	);

	for (const ResourceKey& key : keys) {
		// if this is a default tribe tool, do not store it.
		if (defaultTribeTools.find(key.instanceID) != defaultTribeTools.end()) { continue; }

		int typeID = TribeToolTypeIDFromProp(key);
		tribeToolIDs.insert(make_pair(typeID, key.instanceID));
		tribeToolMetadata.insert(make_pair(typeID, TribeToolMetadataFromProp(key)));
	}
}

TRG_TribePlanManager::ToolMetadata* TRG_TribePlanManager::GetTribeToolMetadata(int toolType) const {
	auto E = tribeToolMetadata.find(toolType);
	if (E != tribeToolMetadata.end()) {
		return E.get_node()->mValue.second;
	}
	return nullptr;
}

// Get the tribe tool data by the tooltype.
// works only for new tools to avoid a callback loop in the detour.
cTribeToolData* TRG_TribePlanManager::GetTribeToolData(int toolType) const {
	using namespace TypeIDs;
	// default tools
	if (toolType <= ToolTypes::DecorPalette) {
		//return Simulator::GetTribeToolData(toolType);
		return nullptr;
	}
	// HB tools
	// NOTE: 'HomePalette' and 'DecorPalette' will never have any real tools associated with it, as it only exists to allow houses to be spawned.
	else {
		//instantiate
		ResourceKey toolKey = ResourceKey(0x0, Names::prop, GroupIDs::TribeToolData);
		int typeIDoverride = -1;

		// Island Event Rare
		if (toolType == EventRare) {
			// TODO: this should maybe do something special that returns the actual model of the rare as well?
			toolKey.instanceID = id("homehut");
			typeIDoverride = toolType;
		}
		// Home Hut
		// Note: The entire range of Housing objects all return the data from tool 11
		else if (toolType >= ToolTypes::HomeStart && toolType <= ToolTypes::HomeEnd) {
			toolKey.instanceID = id("homehut");
			typeIDoverride = toolType;
		}
		// all other tools are pulled from the hashmap
		else if (toolType > HomeEnd && toolType < Decor) {
			auto E = tribeToolIDs.find(toolType);
			if (E != tribeToolIDs.end()) {
				toolKey.instanceID = E.get_node()->mValue.second;
			}
		}
		/*
		// duplicate decor items
		else if (toolType > 99) {
			toolType = abs(toolType);
			//toolType = GetDecorRealToolID(toolType);
			auto E = tribeToolIDs.find(toolType);
			if (E != tribeToolIDs.end()) {
				toolKey.instanceID = E.get_node()->mValue.second;
			}
			typeIDoverride = toolType;
		}*/

		// get the actual data from this file
		return TribeToolDataFromProp(toolKey, typeIDoverride);

	}
	return nullptr;
}

// typeIDoverride forces 'mToolType' of the tribal tool shed to store as this value.
// typeIDoverride = -1 disables the override.
cTribeToolData* TRG_TribePlanManager::TribeToolDataFromProp(ResourceKey key, int typeIDoverride) {
	cTribeToolData* toolData = new(cTribeToolData);
	toolData->mFileKey = key;
	toolData->mMaxNumSlots = 999;

	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(key.instanceID, key.groupID, mpPropList))
	{
		uint32_t tType = -1;
		uint32_t tClass = 0;
		App::Property::GetUInt32(mpPropList.get(), 0x04294750, tType);
		App::Property::GetUInt32(mpPropList.get(), 0x04294751, tClass);
		toolData->mToolType = int(tType);
		toolData->mToolClass = int(tClass);
		App::Property::GetInt32(mpPropList.get(), 0x02166464, toolData->mPrice);
		App::Property::GetFloat(mpPropList.get(), 0x03A26420, toolData->mToolHealth);

		// default all the tool keys to its own file or the last valid health key if unset
		ResourceKey toolKeys;

		// mRackModelKey
		App::Property::GetKey(mpPropList.get(), 0x04294752, toolKeys);
		if (toolKeys.instanceID != 0x0) {
			toolData->mRackModelKey = toolKeys;
		} else { toolData->mRackModelKey = key; }

		// mToolDamageLoKey
		App::Property::GetKey(mpPropList.get(), 0x492D3897, toolKeys);
		if (toolKeys.instanceID != 0x0) {
			toolData->mToolDamageLoKey = toolKeys;
		}
		else { toolData->mToolDamageLoKey = toolData->mRackModelKey; }

		// mToolDamageMdKey
		App::Property::GetKey(mpPropList.get(), 0x492D3896, toolKeys);
		if (toolKeys.instanceID != 0x0) {
			toolData->mToolDamageMdKey = toolKeys;
		}
		else { toolData->mToolDamageMdKey = toolData->mToolDamageLoKey; }

		// mToolDamageHiKey
		App::Property::GetKey(mpPropList.get(), 0x492D3895, toolKeys);
		if (toolKeys.instanceID != 0x0) {
			toolData->mToolDamageHiKey = toolKeys;
		}
		else { toolData->mToolDamageHiKey = toolData->mToolDamageMdKey; }


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

		// use placedToolName for name
		bool hasplacedtext = App::Property::GetText(mpPropList.get(), 0x8CD594D9, toolData->mName);
		if (!hasplacedtext) {
			// if null, use the blockname (palette item name)
			App::Property::GetText(mpPropList.get(), 0x8F6FC401, toolData->mName);
		}
	}
	else {
		return nullptr;
	}

	// if there is a type override, set that as the tool type.
	if (typeIDoverride != -1) {
		toolData->mToolType = typeIDoverride;
	}

	return toolData;
}

int TRG_TribePlanManager::TribeToolTypeIDFromProp(ResourceKey key) {
	uint32_t toolTypeID = -1;

	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(key.instanceID, key.groupID, mpPropList))
	{
		App::Property::GetUInt32(mpPropList.get(), 0x04294750, toolTypeID);
	}
	return toolTypeID;
}

TRG_TribePlanManager::ToolMetadata* TRG_TribePlanManager::TribeToolMetadataFromProp(ResourceKey key) {
	ToolMetadata* meta = new(ToolMetadata);

	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(key.instanceID, key.groupID, mpPropList))
	{
		uint32_t toolHandheldItem = 0;
		App::Property::GetUInt32(mpPropList.get(), 0x6E22D7BF, toolHandheldItem);
		meta->mHandHeldIndex = toolHandheldItem;

		App::Property::GetKey(mpPropList.get(), 0x051CDDBC, meta->mAbilityKey);
		App::Property::GetKey(mpPropList.get(), 0xCEBF7F24, meta->mSpecializedName);
		App::Property::GetKeyInstanceID(mpPropList.get(), 0x5B65C4B6, meta->mHandHeldItemEffect);
		App::Property::GetBool(mpPropList.get(), 0x68575D46, meta->mbToolPrebuilt);
		App::Property::GetBool(mpPropList.get(), 0xA664B3BE, meta->mbToolAllowMultiple);
		App::Property::GetBool(mpPropList.get(), 0xE9B30DC6, meta->mbToolDisableRollover);
		App::Property::GetUInt32(mpPropList.get(), 0x062ECADE, meta->mToolEnRouteAnim);
	}
	return meta;
}

void TRG_TribePlanManager::SetDesiredHandheldItem(cCreatureCitizenPtr citizen) {
	auto meta = GetTribeToolMetadata(citizen->mSpecializedTool);
	if (citizen->mCurrentHandheldItem == meta->mHandHeldIndex) {
		//citizen->mCurrentHandheldItem = citizen->mSpecializedTool;
	}
}

// Get NPC chieftain name path
ResourceKey TRG_TribePlanManager::GetCitizenNameLocaleResource(cCreatureCitizenPtr citizen) const {

	bool chieftain = false;
	{
		auto tribe = citizen->mpOwnerTribe;
		
		if (tribe) {
			auto chieftaincitizen = tribe->GetLeaderCitizen();
			if (chieftaincitizen) {
				chieftain = citizen->mpOwnerTribe->GetLeaderCitizen() == citizen;
			}
		}
		else {
			for (auto item : Simulator::GetData<Simulator::cTribe>()) {
				if (item->GetLeaderCitizen() == citizen) {
					break;
					chieftain = true;
				}
			}
		}
	}
	// citizen is Tool Carrier
	if (!chieftain) {
		if (citizen->mSpecializedTool > HomeEnd) {
			auto meta = GetTribeToolMetadata(citizen->mSpecializedTool);
			if (meta->mSpecializedName.instanceID != 0x0) {
				return meta->mSpecializedName;
			}
		}
		return {};
	}
	// citizen is Chieftain
	else {
		int archetype = citizen->mpOwnerTribe->mTribeArchetype;
		auto res = ResourceKey(0x000000A0, 0x0, id("TribeArchetypes"));

		if (archetype > 0) {
			// Set tribe name from archetype
			/*if (archetype == 15 || archetype == 12) {
				res.instanceID = 0x000000A0; // chieftain
			}*/
			if (archetype == 7) {
				res.instanceID = 0x000000A2; // general
			}
			else if (archetype == 4) {
				res.instanceID = 0x000000A3; // warlord
			}
			else {
				// chieftain (base game)
				res.groupID = 0xF71FA311;
				res.instanceID = 0x04bd540b;
			}
		}
		return res;
	}

}

// Add a chief creature's diet value to the queue
void TRG_TribePlanManager::AddCitizenToolTypeToQueue(int toolID, int handheldID) {
	mToolQueueCitizenData.push_back(Vector2((float)toolID, (float)handheldID));
}

void TRG_TribePlanManager::RemoveCitizenToolTypeQueue() {
	vector<Vector2> new_vector;
	for (size_t i = 1; i < mToolQueueCitizenData.size(); i++) {
		new_vector.push_back(mToolQueueCitizenData[1]);
	}
	mToolQueueCitizenData = new_vector;
}


uint32_t TRG_TribePlanManager::ConvertToolEffectID(uint32_t instanceId) {
	uint32_t tool_id = instanceId;

	if (mToolQueueCitizenData.size() == 0) { return tool_id; }
	Vector2 tooldata_vector = mToolQueueCitizenData[0];

	if (instanceId != 0x0 && tooldata_vector != Vector2(-1,-1)) {

		int tool_index = int(tooldata_vector.x);

		if (tool_index > HomeEnd) {
			int handheld_index = int(tooldata_vector.y);
			uint32_t default_handheld_effect = 0x0; // effect ID

			auto E = defaultTribeToolEffectIDs.find(handheld_index);
			if (E != defaultTribeToolEffectIDs.end()) {
				default_handheld_effect = E.get_node()->mValue.second;
			}

			if (default_handheld_effect != 0x0 && default_handheld_effect == instanceId) {
				auto meta = GetTribeToolMetadata(tool_index);
				if (meta) {
					tool_id = meta->mHandHeldItemEffect;
				}
			}
		}

	}
	RemoveCitizenToolTypeQueue();
	
	return tool_id;
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
					// todo: effect not working
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

	auto meta = GetTribeToolMetadata(toolType);
	// Make tribal huts have to be built
	if (!mbHutsPrebuilt) {
		if (!meta || (meta && !meta->mbToolPrebuilt)) {

			tool->SetHealthPoints(tool->GetMaxHitPoints() * progress_value);
			tool->SetModelKey(plot_social01);

			// do this last
			mpInProgressTools.push_back(tool);
			mpPlotHealths.push_back(tool->mHealthPoints);
		}
	}

	//---------------------------------------------

	// TODO: Allow home type huts to be added until they hit the upper limit for house count, based on tribe tier
	// When the last possible one is spawned, we will have to do some other fancy stuff to dynamically swap IDs on existing homes
	// as you buy and sell them to and from the current limit
	// and also as the tribe tier changes
	// OR
	// Maybe consider just showing or hiding a UI window that grays out the housing slot and says something like
	// (6/6) you need a higher tier tribe for more homes

	if (toolType == HomePalette) {
		tool->SetToolType(GetNewHomeID(tribe));
	}
	else if (meta && meta->mbToolAllowMultiple) {
		//tool->SetToolType(GetNewDecorID(tool->GetToolType(), tribe));
		tool->SetToolType(Decor);
	}
}

void TRG_TribePlanManager::ToolComplete(cTribeToolPtr tool) {
	if (IsToolHome(tool)) {
		//MessageManager.MessageSend(id("UpdateHomes"), nullptr);
	}
	else {
		// debug
		for (auto member : GameNounManager.GetPlayerTribe()->GetTribeMembers()) {
			member->DoAction(kCitizenActionGrabTool, tool.get());
		}
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
	return HomeStart;
}

/*
// EX: tooltype 51 generates a range from 5100 to 5199. the tool type can be derived from this range by dividing by 100 and rounding down
Vector2 TRG_TribePlanManager::GetDecorIDRange(int toolType) const {
	return Vector2((toolType * 100), (toolType * 100 + 99));
}

// EX: tooltype 5106 will return toolType 51
int TRG_TribePlanManager::GetDecorRealToolID(int toolType) const {
	return (int)floor(toolType/100);
}


// Get the nearest open slot for 'Decor' type tools, from DecorStart to DecorEnd
int TRG_TribePlanManager::GetNewDecorID(int toolType, cTribePtr tribe) const {
	Vector2 toolRange = GetDecorIDRange(toolType);
	for (int i = (int)toolRange.x; i <= (int)toolRange.y; i++) {
		if (!tribe->GetToolByType(i)) {
			return i;
		}

	}
	return (int)toolRange.x;
}*/

// get model from tool type and health level
ResourceKey TRG_TribePlanManager::GetPlotModel(cTribeToolPtr tool) const {
	bool modelidx = tool->mHealthPoints > (0.825 * tool->mMaxHealthPoints);

	if (modelidx) { return plot_social02; }
	else { return plot_social01; }

	return tool->GetModelKey();
}

void TRG_TribePlanManager::RemoveFromProgress(cTribeToolPtr tool) {
	for (size_t i = 0; i < mpInProgressTools.size(); i++) {
		if (mpInProgressTools[i] == tool) {
			mpInProgressTools[i] = nullptr;
		}
	}
}

cTribeToolPtr TRG_TribePlanManager::GetHoveredConstructionPlot() const {
	for (auto tool : mpInProgressTools) {
		if (tool && tool->IsRolledOver() && tool->mTribe) {
			return tool;
		}
	}
	return nullptr;
}

bool TRG_TribePlanManager::IsConstructionPlotHovered() const {
	for (auto tool : mpInProgressTools) {
		if (tool && tool->IsRolledOver() && tool->mTribe) {
			return true;
		}
	}
	return false;
}

bool TRG_TribePlanManager::IsToolInProgress(cTribeToolPtr tool) const {
	if (!tool) { return false; }
	for (auto item : mpInProgressTools) {
		if (tool == item && tool->mTribe) {
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
			if (mBabySpawnTask && !mBabySpawnTask->HasExecuted()) {
				Simulator::RemoveScheduledTask(mBabySpawnTask);
			}
			mbBabySpawning = true;
			mBabySpawnTask = Simulator::ScheduleTask(this, &TRG_TribePlanManager::ResetBabySpawningVar, 6.0f);
		}

	}
	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}

// make sure this var doesnt get stuck on
void TRG_TribePlanManager::ResetBabySpawningVar() {
	mbBabySpawning = false;
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