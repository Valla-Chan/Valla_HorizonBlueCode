#include "stdafx.h"
#include "TribeToolManager.h"
#include "TribeToolStratManager.h"

cTribeToolManager::cTribeToolManager()
{
	sInstance = this;

	App::AddUpdateFunction(this);
	WindowManager.GetMainWindow()->AddWinProc(this);
	CursorManager.Load(0xFA09CD25, u"cursor_construct");
	CursorManager.Load(0xE84563D4, u"cursor-object-scale");
	CursorManager.Load(BasicCursorIDs::Cursors::Pickup, u"cursor-crg-pickup");

	PopulateTribeToolData(); // TODO: consider moving this to a post-run detour of the func that pulls the normal tribal tool data, for consistency.
}


cTribeToolManager::~cTribeToolManager()
{
	sInstance = nullptr;
}

cTribeToolManager* cTribeToolManager::sInstance;

cTribeToolManager* cTribeToolManager::Get()
{
	return sInstance;
}

void cTribeToolManager::AttachDetours() {
	TribeTool_SetHoverObjectCursorAndRollover_detour::attach(GetAddress(cTribeInputStrategy, SetHoverObjectCursorAndRollover));
}

// For internal use, do not modify.
int cTribeToolManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cTribeToolManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cTribeToolManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(cTribeToolManager);
	return nullptr;
}

void cTribeToolManager::Update()
{
	if (IsTribeGame()) {

		UpdateToolsProgress();

		//// Handle manual hovered rollover
		//// This must go last
		auto hovered = GameViewManager.GetHoveredObject();
		if (hovered) {
			// Tool
			auto tribetool = object_cast<cTribeTool>(hovered);
			// Only display info for tools from our tribe, when not in combat.
			if (tribetool && tribetool->mTribe == GameNounManager.GetPlayerTribe()) {
				UI::SimulatorRollover::ShowRollover(tribetool);
				return;
			}
		}

	}
}

//-------------------------------------------------------------------------------
// TOOLDATA

// Read and store the new tool files from the folder in a vector, like the normal tool code does.
void cTribeToolManager::PopulateTribeToolData() {

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
		mTribeToolIDs.insert(make_pair(typeID, key.instanceID));
		auto meta = TribeToolMetadataFromProp(key);
		mTribeToolMetadata.insert(make_pair(typeID, meta));
		if (meta && meta->mToolStrategyID != 0x0) {
			TribeToolStratManager.SetToolIDStrategy(typeID, meta->mToolStrategyID);
		}
	}
}

cTribeToolManager::ToolMetadata* cTribeToolManager::GetTribeToolMetadata(cTribeToolPtr tool) const {
	return GetTribeToolMetadata(tool->GetToolType());
}

cTribeToolManager::ToolMetadata* cTribeToolManager::GetTribeToolMetadata(int toolType) const {
	auto E = mTribeToolMetadata.find(toolType);
	if (E != mTribeToolMetadata.end()) {
		return E.get_node()->mValue.second;
	}
	return nullptr;
}

// Get the tribe tool data by the tooltype.
// works only for new tools to avoid a callback loop in the detour.
cTribeToolData* cTribeToolManager::GetTribeToolData(int toolType) const {
	// default tools
	if (toolType <= ToolTypes::DecorPalette) {
		// Get this data from the tribe instead.
		return nullptr;
	}
	// HB tools
	// NOTE: 'HomePalette' will never have any ingame tools associated with it, as it only exists to allow houses to be spawned.
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
			auto E = mTribeToolIDs.find(toolType);
			if (E != mTribeToolIDs.end()) {
				toolKey.instanceID = E.get_node()->mValue.second;
			}
		}
		/*
		// duplicate decor items
		else if (toolType > 99) {
			toolType = abs(toolType);
			//toolType = GetDecorRealToolID(toolType);
			auto E = mTribeToolIDs.find(toolType);
			if (E != mTribeToolIDs.end()) {
				toolKey.instanceID = E.get_node()->mValue.second;
			}
			typeIDoverride = toolType;
		}*/

		// get the actual data from this file
		return TribeToolDataFromProp(toolKey, typeIDoverride);

	}
	return nullptr;
}

//// Pull TOOL DATA
// typeIDoverride forces 'mToolType' of the tribal tool shed to store as this value.
// typeIDoverride = -1 disables the override.
cTribeToolData* cTribeToolManager::TribeToolDataFromProp(ResourceKey key, int typeIDoverride) {
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
		}
		else { toolData->mRackModelKey = key; }

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


// Pull TYPE IDs
int cTribeToolManager::TribeToolTypeIDFromProp(ResourceKey key) {
	uint32_t toolTypeID = -1;

	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(key.instanceID, key.groupID, mpPropList))
	{
		App::Property::GetUInt32(mpPropList.get(), 0x04294750, toolTypeID);
	}
	return toolTypeID;
}


// Pull METADATA
cTribeToolManager::ToolMetadata* cTribeToolManager::TribeToolMetadataFromProp(ResourceKey key) {
	ToolMetadata* meta = new(ToolMetadata);

	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(key.instanceID, key.groupID, mpPropList))
	{
		// Strategy
		App::Property::GetKeyInstanceID(mpPropList.get(), 0x9BB3CA2B, meta->mToolStrategyID);

		// Handheld Items
		{
			uint32_t toolHandheldItem = 0;
			App::Property::GetUInt32(mpPropList.get(), 0x6E22D7BF, toolHandheldItem);
			meta->mHandHeldIndex = toolHandheldItem;
			App::Property::GetKeyInstanceID(mpPropList.get(), 0x5B65C4B6, meta->mHandHeldItemEffect);
		}

		// FX and anims
		App::Property::GetUInt32(mpPropList.get(), 0x062ECADE, meta->mToolEnRouteAnim);
		App::Property::GetUInt32(mpPropList.get(), 0x062ECADE, meta->mToolInteractAnim);

		// UI / Cursor
		{
			string16 cursor;
			// Active Cursor
			if (App::Property::GetString16(mpPropList.get(), 0xD265DD92, cursor)) {
				meta->mToolActiveCursorID = id(cursor.c_str());
				CursorManager.Load(meta->mToolActiveCursorID, cursor.c_str());
			}
			// Inactive Cursor
			if (App::Property::GetString16(mpPropList.get(), 0x0BF57AF7, cursor)) {
				meta->mToolInactiveCursorID = id(cursor.c_str());
				CursorManager.Load(meta->mToolInactiveCursorID, cursor.c_str());
			}
			// Invalid Cursor
			if (App::Property::GetString16(mpPropList.get(), 0x9A846D51, cursor)) {
				meta->mToolInvalidCursorID = id(cursor.c_str());
				CursorManager.Load(meta->mToolInvalidCursorID, cursor.c_str());
			}
		}

		// Scale
		{
			// TODO: this is totally broken for some reason. fix this later.
			/*
			float scalebound_lower = 0.0f;
			float scalebound_upper = 0.0f;
			
			if (App::Property::GetFloat(mpPropList.get(), 0xF023ED73, scalebound_lower) && scalebound_lower > 0.0f) { // modelMinScale
				meta->mToolScaleBounds.x = scalebound_lower;
			}
			if (App::Property::GetFloat(mpPropList.get(), 0xF023ED79, scalebound_upper) && scalebound_upper > 0.0f) { // modelMaxScale
				meta->mToolScaleBounds.y = scalebound_upper;
			}
			*/
		}
		
		// Bools
		App::Property::GetBool(mpPropList.get(), 0x68575D46, meta->mbToolPrebuilt);
		App::Property::GetBool(mpPropList.get(), 0xA664B3BE, meta->mbToolAllowMultiple);
		App::Property::GetBool(mpPropList.get(), 0xE9B30DC6, meta->mbToolDisableRollover);
		
		// Ability and specialized name
		App::Property::GetKey(mpPropList.get(), 0x051CDDBC, meta->mAbilityKey);
		App::Property::GetKey(mpPropList.get(), 0xCEBF7F24, meta->mSpecializedName);
	}
	return meta;
}

// Get the nearest open slot for 'Home' type huts, from HomeStart to HomeEnd
int cTribeToolManager::GetNewHomeID(cTribePtr tribe) {
	for (int i = HomeStart; i <= HomeEnd; i++) {
		if (!tribe->GetToolByType(i)) {
			return i;
		}

	}
	return HomeStart;
}

//-------------------------------------------------------------------------------
// HELPER FUNCTIONS
//-------------------------------------------------------------------------------

// Tribe Plan

bool cTribeToolManager::TribeHasToolType(cTribePtr tribe, int tooltype) {
	if (tribe && tribe->GetToolByType(tooltype)) {
		return true;
	}
	return false;
}

bool cTribeToolManager::IsPlannerOpen() {
	if (IsTribeGame()) {
		auto window = WindowManager.GetMainWindow();
		auto plannerUI = window->FindWindowByID(0x93019DBC);

		if (plannerUI && plannerUI->IsVisible()) { return true; }
	}
	return false;
}

bool cTribeToolManager::HoveringScalableObject() {

	if (IsTribeGame() && mbShiftHeld) {
		auto hovered = GameViewManager.GetHoveredObject();
		if (hovered) {
			auto hovered_tool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());
			if (hovered_tool && IsToolPlayerOwned(hovered_tool) && !IsToolFirePit(hovered_tool) && !IsToolInProgress(hovered_tool)) {
				return true;
			}
			else {
				auto hovered_hut = object_cast<Simulator::cTribeHut>(GameViewManager.GetHoveredObject());
				if (hovered_hut && IsHutPlayerOwned(hovered_hut) && hovered_hut->mHealthPoints > 0) {
					return true;
				}
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------
// Tools

cTribeToolPtr cTribeToolManager::GetHoveredTool(bool playerOwned) {
	auto hoveredTool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());
	if (playerOwned) {
		if (hoveredTool && hoveredTool->mTribe == GameNounManager.GetPlayerTribe()) {
			return hoveredTool;
		}
		else {
			return nullptr;
		}
	}
	return hoveredTool;
}

void cTribeToolManager::ScaleTribeTool(cTribeToolPtr tool, float amount) {
	Vector2 scalebounds = mTribeToolScaleBounds;
	auto meta = GetTribeToolMetadata(tool->GetToolType());
	if (meta) {
		scalebounds = meta->mToolScaleBounds;
	}
	float newscale = clamp(tool->GetScale() + amount, scalebounds.x, scalebounds.y);
	if (newscale != tool->GetScale()) {
		tool->SetScale(newscale);
		PlayBuildingScaleSound();
	}
}

void cTribeToolManager::ScaleTribeHut(cTribeHutPtr hut, float amount) {
	Vector2 scalebounds = mTribeHutScaleBounds;
	float newscale = clamp(hut->GetScale() + amount, scalebounds.x, scalebounds.y);

	if (newscale != hut->GetScale()) {
		hut->SetScale(newscale);
		PlayBuildingScaleSound();
	}
}

void cTribeToolManager::PlayBuildingScaleSound() {
	int sndidx = rand(41) + 1;
	/*
	eastl::string16 str;
	if (sndidx < 10) {
		str.sprintf(u"building_part_size0%i", sndidx);
	}
	else {
		str.sprintf(u"building_part_size%i", sndidx);
	}*/
	//Audio::PlayAudio(id(str.c_str()));
	Audio::PlayAudio(id("building_scale_low"));
}

// TODO: this!
bool cTribeToolManager::CanToolBeUsed(cTribeToolPtr tool) {
	return true;
	//tool->data
}


bool cTribeToolManager::IsToolFirePit(cTribeToolPtr tool) {
	if (!tool) { return false; }
	else {
		return (tool->GetToolType() == 10);
	}
	/*
	switch (tool->GetModelKey().instanceID) {
	case 0x1AE006AB:
	case 0x1AE006A8:
	case 0x1AE006A9:
		return true;
		break;
	}*/
	return false;
}

bool cTribeToolManager::IsToolHome(cTribeToolPtr tool) {
	auto toolType = tool->GetToolType();
	return (toolType >= HomeStart && toolType <= HomeEnd);
}

bool cTribeToolManager::IsToolPlayerOwned(cTribeToolPtr tool) {
	return (tool->mTribe == GameNounManager.GetPlayerTribe());
}
bool cTribeToolManager::IsHutPlayerOwned(cTribeHutPtr hut) {
	return (hut->mpTribe == GameNounManager.GetPlayerTribe());
}

bool cTribeToolManager::IsNewTool(cTribeToolPtr tool, bool includehomes, bool includerares, bool includedecor) {
	auto type = tool->GetToolType();
	if (type > HomeEnd && type < Decor) {
		return true;
	}
	if (type >= EventRare && type < Decor) {
		if (includerares && type == EventRare) {
			return true;
		}
		else if (includehomes && type >= HomeStart && type < HomeEnd) {
			return true;
		}
		else if (includedecor && type == Decor) {
			return true;
		}
	}
	return false;
}


//--------------------------------------------------------------------------------------------
// Tribe Members

bool cTribeToolManager::HasSelectedMembers() {
	vector<cCreatureCitizenPtr> selected;
	auto tribe = GameNounManager.GetPlayerTribe();
	eastl::vector<cCreatureCitizenPtr>& members = tribe->GetSelectableMembers();
	for (size_t i = 0; i < members.size(); i++) {
		// Try to prevent some odd crashes here...
		// TODO: still crashing!!
		if (members[i] && members[i]->mpOwnerTribe && members[i]->IsSelected()) {
			return true;
		}
	}
	return false;
}

vector<cCreatureCitizenPtr> cTribeToolManager::GetSelectedMembers() {
	vector<cCreatureCitizenPtr> selected;
	auto tribe = GameNounManager.GetPlayerTribe();
	eastl::vector<cCreatureCitizenPtr>& members = tribe->GetSelectableMembers();
	for (size_t i = 0; i < members.size(); i++) {
		if (members[i]->IsSelected()) {
			selected.push_back(members[i]);
		}
	}
	return selected;
}

void cTribeToolManager::UseTool(vector<cCreatureCitizenPtr> citizens, cTribeToolPtr tool) {
	for (size_t i = 0; i < citizens.size(); i++) {
		UseTool(citizens[i], tool);
	}
}

/// Tell a citizen creature to use a tool, if they are part of the same tribe.
void cTribeToolManager::UseTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool) {
	if (citizen->mpOwnerTribe == tool->mTribe) {
		citizen->DoAction(kCitizenActionGrabTool, tool->ToGameData());
	}
}

/// Tell the selected citizen creatures to use a tool, if they are part of the same tribe.
void cTribeToolManager::SelectedUseTool(cTribeToolPtr tool) {

	if (CanToolBeUsed(tool)) {
		auto selected = GetSelectedMembers();
		if (selected.size() > 0) {
			UseTool(selected, tool);
		}
	}
}


//--------------------------------------------------------------------------------------------
// Contruction Progress

void cTribeToolManager::UpdateToolsProgress() {
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
					effectPoof->Start(0);
					// todo: effect not workin? may be fixed
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

void cTribeToolManager::RemoveFromProgress(cTribeToolPtr tool) {
	for (size_t i = 0; i < mpInProgressTools.size(); i++) {
		if (mpInProgressTools[i] == tool) {
			mpInProgressTools[i] = nullptr;
		}
	}
}

ResourceKey cTribeToolManager::GetPlotModel(cTribeToolPtr tool) const {
	bool modelidx = tool->mHealthPoints > (0.825 * tool->mMaxHealthPoints);

	if (modelidx) { return plot_social02; }
	else { return plot_social01; }

	return tool->GetModelKey();
}

cTribeToolPtr cTribeToolManager::GetHoveredConstructionPlot() const {
	for (size_t i = 0; i < mpInProgressTools.size(); i++) {
		if (mpInProgressTools[i] && mpInProgressTools[i]->IsRolledOver() && mpInProgressTools[i]->mTribe) {
			return mpInProgressTools[i];
		}
	}
	return nullptr;
}

bool cTribeToolManager::IsConstructionPlotHovered() const {
	for (size_t i = 0; i < mpInProgressTools.size(); i++) {
		if (mpInProgressTools[i] && mpInProgressTools[i]->IsRolledOver() && mpInProgressTools[i]->mTribe) {
			return true;
		}
	}
	return false;
}

bool cTribeToolManager::IsToolInProgress(cTribeToolPtr tool) const {
	if (!tool) { return false; }
	for (size_t i = 0; i < mpInProgressTools.size(); i++) {
		if (tool == mpInProgressTools[i] && tool->mTribe) {
			return true;
		}
	}
	return false;
}

bool cTribeToolManager::IsToolDamaged(cTribeToolPtr tool) const {
	if (!tool) { return false; }
	return tool->mHealthPoints < tool->mMaxHealthPoints;
}

void cTribeToolManager::AddedTool(cTribeToolPtr tool, cTribePtr tribe) {
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

void cTribeToolManager::ToolComplete(cTribeToolPtr tool) {
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

//--------------------------------------------------------------------------------------------
// Handheld Items

// Add a non-chieftain citizen creature's tool info to the queue
void cTribeToolManager::AddCitizenToolTypeToQueue(int toolID, int handheldID) {
	mToolQueueCitizenData.push_back(Vector2((float)toolID, (float)handheldID));
}

void cTribeToolManager::RemoveCitizenToolTypeQueue() {
	vector<Vector2> new_vector;
	for (size_t i = 1; i < mToolQueueCitizenData.size(); i++) {
		new_vector.push_back(mToolQueueCitizenData[i]);
	}
	mToolQueueCitizenData = new_vector;
}

uint32_t cTribeToolManager::ConvertToolEffectID(uint32_t instanceId) {
	uint32_t tool_id = instanceId;

	if (mToolQueueCitizenData.size() == 0) { return tool_id; }
	Vector2 tooldata_vector = mToolQueueCitizenData[0];

	if (instanceId != 0x0 && tooldata_vector != Vector2(-1, -1)) {

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

// Get citizen name path
ResourceKey cTribeToolManager::GetCitizenNameLocaleResource(cCreatureCitizenPtr citizen) const {

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


//--------------------------------------------------------------------------------------------
// Messages

int cTribeToolManager::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

bool cTribeToolManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsTribeGame()) {
		return false;
	}

	if (IsPlannerOpen()) {

		if (message.Key.vkey == VK_SHIFT || message.IsType(kMsgMouseMove)) {
			if (message.Key.vkey == VK_SHIFT) {
				if (message.IsType(kMsgKeyDown)) {
					mbShiftHeld = true;
				}
				else if (message.IsType(kMsgKeyUp)) {
					mbShiftHeld = false;
				}
			}

			if (HoveringScalableObject()) {
				CursorManager.SetActiveCursor(0xE84563D4);
			}

		}


		

		// Mouse wheel scrolled in planner
		if (message.IsType(kMsgMouseWheel) && mbShiftHeld && message.MouseWheel.wheelDelta != 0) {
			auto hovered_tool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());

			// Allow scaling hovered tool if it belongs to your tribe
			// do not allow scaling if not yet built or is a firepit.
			// TODO: later, if needed, prevent the item from being scaled if it is an event item
			if (hovered_tool && IsToolPlayerOwned(hovered_tool) && !IsToolFirePit(hovered_tool) && !IsToolInProgress(hovered_tool)) {
				float scrollamt = (message.MouseWheel.wheelDelta / 120.0f) * 0.1f;
				ScaleTribeTool(hovered_tool, scrollamt);
			}
			// Scaling the tribal hut
			// Do we want this to be possible? how do we handle the standard scale?
			// This will all need to be configured later.
			else {
				auto hovered_hut = object_cast<Simulator::cTribeHut>(GameViewManager.GetHoveredObject());
				
				if (hovered_hut && IsHutPlayerOwned(hovered_hut) && hovered_hut->mHealthPoints > 0) {
					float scrollamt = (message.MouseWheel.wheelDelta / 120.0f) * 0.2f;
					ScaleTribeHut(hovered_hut, scrollamt);
				}
			}
		}
	}

	return false;
}
