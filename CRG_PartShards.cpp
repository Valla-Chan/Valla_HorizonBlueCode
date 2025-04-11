﻿#include "stdafx.h"
#include "CRG_PartShards.h"
#include <Spore\UTFWin\IText.h>

CRG_PartShards::CRG_PartShards()
{
	WindowManager.GetMainWindow()->AddWinProc(this);

	SimulatorSystem.AddStrategy(this, NOUN_ID);
}

CRG_PartShards::~CRG_PartShards()
{
}

int CRG_PartShards::AddRef() {
	return Simulator::cStrategy::AddRef();
}
int CRG_PartShards::Release() {
	return Simulator::cStrategy::Release();
}

const char* CRG_PartShards::GetName() const {
	return "Valla_HorizonBlue::CRG_PartShards";
}

bool CRG_PartShards::Write(Simulator::ISerializerStream* stream)
{
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool CRG_PartShards::Read(Simulator::ISerializerStream* stream)
{
	mShards = 0;
	mDestroyedParts.clear();
	mMouthsDestroyed = 0;

	auto value = Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
	return value;
}

/// END OF AUTOGENERATED METHODS ///
////////////////////////////////////

Simulator::Attribute CRG_PartShards::ATTRIBUTES[] = {
	// Add more attributes here
	SimAttribute(CRG_PartShards,mShards,1),
	SimAttribute(CRG_PartShards,mDestroyedParts,2),
	// This one must always be at the end
	Simulator::Attribute()
};

// You can extend this function to return any other types your class implements.
void* CRG_PartShards::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(CRG_PartShards);
	return nullptr;
}


void CRG_PartShards::Initialize() {
}

void CRG_PartShards::Dispose() {
	
}

void CRG_PartShards::Update(int deltaTime, int deltaGameTime) {

}

//-------------------------------------------------------------------------------------


void CRG_PartShards::AddShards(int shardcount) {
	SetShardCount(mShards + shardcount);
}

void CRG_PartShards::SubtractShards(int shardcount) {
	AddShards(-shardcount);
}

void CRG_PartShards::SetShardCount(int shardcount) {
	mPrevShards = mShards;
	mShards = shardcount;
	UpdateShardCount();
}

int  CRG_PartShards::GetShardCount() const {
	return mShards;
}

// TODO: eventually we may have different tiers of shards that are unlocked, based on the price.
bool CRG_PartShards::HasEnoughShards() const {
	// buy parts back
	if (ArePartsSelected()) {
		return mShards >= GetAllPartsShardBuybackCost();
	}
	// gamble for new part
	else {
		return mShards >= mMinShardsToGamble;
	}
	
}

//-----------------------------------------------------------------------------------------
// Shards UI


// For these next 2 funcs,
// maybe figure out some way to make the text swell and animate to the new number,
// like how spore does with budgets.

// update shard count whenever opening the collections interface. (it is preserved when switching tabs)
void CRG_PartShards::UpdateShardCount() {
	if (IsCollectionsOpen()) {
		auto shardholder = WindowManager.GetMainWindow()->FindWindowByID(0x895DA0C1);
		auto text_shardcount = shardholder->FindWindowByID(0xD6108B1A);
		
		eastl::string16 str;
		str.sprintf(u"%i", mShards);
		text_shardcount->SetCaption(str.c_str());

		UpdateGambleButtonState();
	}
}

void CRG_PartShards::UpdatePartValue() {
	if (IsCollectionsOpen()) {
		auto shardholder = WindowManager.GetMainWindow()->FindWindowByID(0x895DA0C1);
		auto text_partvalue = shardholder->FindWindowByID(0xD6108B1B);

		eastl::string16 str;
		if (ArePartsSelected()) {
			if (mSelectionType == Sell) {
				str.sprintf(u"%i", GetAllPartsShardValue());
			}
			else {
				str.sprintf(u"%i", GetAllPartsShardBuybackCost());
			}
			
		}
		else {
			str.sprintf(u"%i", mMinShardsToGamble);
		}
		text_partvalue->SetCaption(str.c_str());

		UpdateGambleButtonState();
	}
}

void CRG_PartShards::UpdateGambleButtonState() {
	if (mSelectedParts.size() > 0) {
		if (mSelectionType == Sell) {
			SetGambleButtonState(Extract);
		}
		else {
			SetGambleButtonState(Unlock);
		}
	}
	else {
		SetGambleButtonState(Redeem);
	}
}

void CRG_PartShards::SetGambleButtonState(int state) {
	auto shardholder = WindowManager.GetMainWindow()->FindWindowByID(0x895DA0C1);
	if (!shardholder) { return; }
	auto button_gamble = shardholder->FindWindowByID(0x036AD4A4);
	auto text_partcost = shardholder->FindWindowByID(0xD0998439);
	
	// Extract (scrap parts for shards)
	if (state == Extract) {
		bool allmouthparts = AreAllMouthsSelected();
		if (!allmouthparts) {
			GetWindowTooltip(button_gamble)->mText = localeShardsExtractDesc.GetText();
		}
		else {
			GetWindowTooltip(button_gamble)->mText = localeShardsExtractDescInvalid.GetText();
		}
		SetGambleButtonEnabled(!allmouthparts);
		button_gamble->SetCaption(localeShardsExtractTxt.GetText());

		text_partcost->SetCaption(localePartValueTxt.GetText());
	}

	// Redeem (unlock rand part)
	else if (state == Redeem) {
		// is shard cost sufficient
		if (HasEnoughShards()) { SetGambleButtonEnabled(true); }
		else { SetGambleButtonEnabled(false); }

		button_gamble->SetCaption(localeShardsRedeemTxt.GetText());
		GetWindowTooltip(button_gamble)->mText = localeShardsRedeemDesc.GetText();

		// Show how much money you will be gambling for the part.
		text_partcost->SetCaption(localePartRedeemTxt.GetText());
	}

	// Unlock (unlock scrapped part)
	else if (state == Unlock) {
		// is shard cost sufficient
		if (HasEnoughShards()) {
			SetGambleButtonEnabled(true);
			GetWindowTooltip(button_gamble)->mText = localeShardsUnlockDesc.GetText();
		}
		else {
			SetGambleButtonEnabled(false);
			GetWindowTooltip(button_gamble)->mText = localeShardsUnlockDescInvalid.GetText();
		}

		// single part or multiple parts
		if (mSelectedParts.size() == 1) {
			button_gamble->SetCaption(localeShardsUnlockTxt.GetText());
		}
		else { button_gamble->SetCaption(localeShardsUnlockTxt2.GetText()); }
		
		text_partcost->SetCaption(localePartCostTxt.GetText());
	}
}

void CRG_PartShards::SetGambleButtonEnabled(bool state) {
	auto shardholder = WindowManager.GetMainWindow()->FindWindowByID(0x895DA0C1);
	if (!shardholder) { return; }
	auto button_gamble = shardholder->FindWindowByID(0x036AD4A4);

	button_gamble->SetEnabled(state);
	auto inflate = GetWindowInflate(button_gamble);
	if (state) {
		inflate->SetScale(1.05f);
	}
	else {
		inflate->SetScale(1.0f);
	}
}

void CRG_PartShards::GambleButtonPressed() {
	if (ArePartsSelected()) {
		if (mSelectionType == Sell) {
			// destroy parts, add shards
			DestroySelectedParts();
		}
		else if (HasEnoughShards()) {
			UnlockSelectedParts();
		}
	}
	else {
		// redeem shards for random part
		if (HasEnoughShards()) {
			RedeemPart(1, mMinShardsToGamble);
		}
		
	}
}

//-----------------------------------------------------------------------------------------
// Part Redeem / Destroy

void CRG_PartShards::RedeemPart(int tier, int price) {
	App::CreatureModeStrategies::UnlockPart action = { GameNounManager.GetAvatar(), 0, tier };
	CreatureModeStrategy.ExecuteAction(action.ID, &action);
	SubtractShards(price);
	// Make item UI pop up
	// TODO: add to current collection window if open, or highlight added categories
	App::ScheduleTask(this, &CRG_PartShards::ShowNewPartUnlock, 0.01f);
}

void CRG_PartShards::ShowNewPartUnlock() {
	if (IsCollectionsOpen()) {
		auto partID = GetPlayer()->mpCRGItems->mUnlockedItems.back();
		ResourceKey partKey = ResourceKey(partID.instanceID, TypeIDs::Names::prop, partID.groupID);
		SetPartUnlockWindowState(true, partKey);
	}
}

void CRG_PartShards::ClosePartUnlockWindow() {
	SetPartUnlockWindowState(false);
}

void CRG_PartShards::SetPartUnlockWindowState(bool state, ResourceKey partkey) {
	auto partunlock = WindowManager.GetMainWindow()->FindWindowByID(0x895DA0CE);


	// closing
	if (!state) {
		//GetWindowGlide(partunlock)->SetTime(2);
	}
	// opening
	else if (partkey.instanceID != 0x0) {
		//GetWindowGlide(partunlock)->SetTime(0.4);
		auto text_partname = partunlock->FindWindowByID(0xF554489D);
		auto img_particon = partunlock->FindWindowByID(0x9554489F);

		LocalizedString name = CapabilityChecker.GetModelBlockName(partkey);
		ResourceKey icon = ResourceKey(partkey.instanceID, TypeIDs::Names::png, 0x02231C8B); // creatureparticons~

		text_partname->SetCaption(name.GetText());
		Image::SetBackgroundByKey(img_particon, icon);
		
		if (schedule_closepartunlock && !schedule_closepartunlock->HasExecuted()) {
			App::RemoveScheduledTask(schedule_closepartunlock);
		}
		schedule_closepartunlock = App::ScheduleTask(this, &CRG_PartShards::ClosePartUnlockWindow, 1.5f);
	}
	partunlock->SetVisible(state);
}



bool CRG_PartShards::IsPartDestroyed(ResourceKey partkey) const {
	for (size_t i = 0; i < mDestroyedParts.size(); i++) {
		if (mDestroyedParts[i].instanceID == partkey.instanceID) {
			return true;
		}
	}
	return false;
}

bool CRG_PartShards::IsPartUsedOnCreature(ResourceKey partkey, cCreatureBase* creature) const {
	return CapabilityChecker.IsPartOnCreature(creature, partkey);
}

bool CRG_PartShards::IsPartMouth(ResourceKey partkey) const {
	bool mouthcap = CapabilityChecker.GetModelIntValue(partkey, 0xB00F0FEC) >= 1;
	bool callcap = CapabilityChecker.GetModelIntValue(partkey, 0xF354A87A) >= 1;
	return mouthcap && callcap;
}

bool CRG_PartShards::IsPartWing(ResourceKey partkey) const {
	bool glidecap = CapabilityChecker.GetModelIntValue(partkey, 0x04F4E1B4) >= 1;
	bool wingcap = CapabilityChecker.GetModelIntValue(partkey, 0x04F4D188) >= 1;
	return glidecap && wingcap;
}

// part is a functionless or detail part
bool CRG_PartShards::IsPartDetail(ResourceKey partkey) const {
	uint32_t rigblocktype = CapabilityChecker.GetModelKeyInstanceID(partkey, 0x0186609D);
	
	// detail, ear, nose, antenna
	if (rigblocktype == 0x5F0A4B8A || rigblocktype == 0x2E9313CF || rigblocktype == 0x39A35050 || rigblocktype == 0x6E91F0BE) {
		return true;
	}

	return false;
}

// part is a hand, foot, or weapon
bool CRG_PartShards::IsPartHandFootWeapon(ResourceKey partkey) const {
	uint32_t rigblocktype = CapabilityChecker.GetModelKeyInstanceID(partkey, 0x0186609D);
	
	if (rigblocktype == 0xDD204963 || rigblocktype == 0xB4B1179D) {
		return true;
	}
	else {
		if (CapabilityChecker.GetModelIntValue(partkey, 0xB1C3E5B8) >= 1) { return true; }; // charge
		if (CapabilityChecker.GetModelIntValue(partkey, 0xB1C3E5C0) >= 1) { return true; }; // strike
		if (CapabilityChecker.GetModelIntValue(partkey, 0xB1C3E5B9) >= 1) { return true; }; // spit
	}

	return false;
}

// part is a limb assembly
bool CRG_PartShards::IsPartLimb(ResourceKey partkey) const {
	bool glidecap = CapabilityChecker.GetModelIntValue(partkey, 0x04F4E1B4) >= 1;
	bool wingcap = CapabilityChecker.GetModelIntValue(partkey, 0x04F4D188) >= 1;
	return glidecap && wingcap;
}

const eastl::hash_set<uint32_t> cellSpikeParts = {
	id("ce_cell_weapon_spike_01-symmetric"),
	id("ce_cell_weapon_spike_02-symmetric"),
	id("ce_cell_weapon_spike_03-symmetric"),
	id("ce_cell_weapon_spike_04-symmetric"),
	//id("ce_cell_weapon_spike_01"),
	//id("ce_cell_weapon_spike_02"),
	//id("ce_cell_weapon_spike_03"),
	//id("ce_cell_weapon_spike_04"),
	id("DS-ce_cell_weapon_spike_01-symmetric"),
	id("DS-ce_cell_weapon_spike_02-symmetric"),
	id("DS-ce_cell_weapon_spike_03-symmetric"),
	id("DS-ce_cell_weapon_spike_04-symmetric"),
};

bool CRG_PartShards::IsPartCellPart(ResourceKey partkey) const {
	// TODO: eventually, create a simple bool to add into all the cell parts via UPE that users can add into their own modded cell parts. for now though, do this.

	uint32_t parentkey = CapabilityChecker.GetModelParentKey(partkey).instanceID;

	// filter, jaw, poker
	if (parentkey == 0x250CFC9B || parentkey == 0x817CEE91 || parentkey == 0x36862158) {
		return true;
	}
	else {
		if (CapabilityChecker.GetModelIntValue(partkey, 0x11B79A73) >= 1) { return true; }; // modelCapabilityCellEye
		if (CapabilityChecker.GetModelIntValue(partkey, 0x11B79302) >= 1) { return true; }; // modelCapabilityCellMovement
		if (CapabilityChecker.GetModelIntValue(partkey, 0x11B79304) >= 1) { return true; }; // modelCapabilityCellWeaponCharging
		if (CapabilityChecker.GetModelIntValue(partkey, 0xB1C3E5B9) >= 1) { return true; }; // modelCapabilityCellWeaponCharging

		// spike parts do not fall under any of this, check them manually. :/
		auto partID = partkey.instanceID;
		if (cellSpikeParts.find(partID) != cellSpikeParts.end()) { return true; }
	}
	return false;
}


int CRG_PartShards::GetPartUnlockLevel(ResourceKey partkey) const {
	return CapabilityChecker.GetPartUnlockLevel(partkey);
}

void CRG_PartShards::DestroySelectedParts() {
	for (size_t i = 0; i < mSelectedParts.size(); i++) {
		if (!IsPartDestroyed(mSelectedParts[i])) {
			mDestroyedParts.push_back(mSelectedParts[i]);
			if (IsPartMouth(mSelectedParts[i])) {
				mMouthsDestroyed++;
			}
		}
	}
	for (size_t i = 0; i < mSelectedPartWindows.size(); i++) {
		SetPartWindowUnlocked(mSelectedPartWindows[i], false);
	}


	AddShards(GetAllPartsShardValue());
	ClearSelectedParts();
	Audio::PlayAudio(id("editor_shardsell"));
}

void CRG_PartShards::ClearSelectedParts() {

	// Disable the part window selection
	for (size_t i = 0; i < mSelectedPartWindows.size(); i++) {
		SetPartWindowSelected(mSelectedPartWindows[i], false);
	}

	// Last
	ClearSelectionSimple();
}

void CRG_PartShards::ClearSelectionSimple() {
	mSelectedParts.clear();
	mSelectedPartWindows.clear();
	mMouthsSelected = 0;
	UpdatePartValue();
}


void CRG_PartShards::UnlockSelectedParts() {

	if (mSelectionType != Buy) { return; }

	// Enable the part windows
	for (size_t i = 0; i < mSelectedPartWindows.size(); i++) {
		SetPartWindowUnlocked(mSelectedPartWindows[i], true);
	}
	// reform the mDestroyedParts array without these parts
	vector<ResourceKey> mDestroyedPartsNew;
	for (size_t i = 0; i < mDestroyedParts.size(); i++) {
		bool found = false;
		for (size_t j = 0; j < mSelectedParts.size(); j++) {
			if (mDestroyedParts[i] == mSelectedParts[j]) {
				if (IsPartMouth(mSelectedParts[j])) {
					mMouthsDestroyed--;
				}
				found = true;
				break;
			}
		}
		if (!found) {
			mDestroyedPartsNew.push_back(mDestroyedParts[i]);
		}
	}
	mDestroyedParts = mDestroyedPartsNew;


	// Last
	SubtractShards(GetAllPartsShardBuybackCost());
	ClearSelectionSimple();
	Audio::PlayAudio(id("part_unlock00"));
}

//-----------------------------------------------------------------------------------------
// Collections UI

void CRG_PartShards::AddPartWindow(IWindow* pWindow, const ResourceKey& imageName) {
	auto part = GetPartKeyFromID(imageName.instanceID);
	// if the part is a mouth, put it in a special vector
	bool ismouth = IsPartMouth(part);
	if (ismouth) {
		mMouthParts.push_back(part);
	}
	
	if (IsPartDestroyed(imageName)) {
		SetPartWindowUnlocked(pWindow->GetParent(), false);
		if (ismouth) {
			mMouthsDestroyed++;
		}
	}
	else if (IsPartUsedOnCreature(imageName, GameNounManager.GetAvatar())) {
		SetPartWindowUnlocked(pWindow->GetParent(), true);
	}
	
}

void CRG_PartShards::TryCollectionsTabOpened() {
	if (!mbSupressCollectionsOpened) {
		mbSupressCollectionsOpened = true;
		CollectionsTabOpened();
		App::ScheduleTask(this, &CRG_PartShards::ResetCollectionsOpenedBool, 0.00001f);
	}
}

// switching tabs
void CRG_PartShards::CollectionsTabOpened() {
	// first time open for this save
	if (!mbCollectionsLoadedOnce) {
		mbCollectionsLoadedOnce = true;
		
		UpdateShardCount();
		UpdatePartValue();
	}
	mMouthParts.clear();
	mMouthsSelected = 0;
	mMouthsDestroyed = 0;
	// clear selection
	ClearSelectionSimple();
}

void CRG_PartShards::ResetCollectionsOpenedBool() {
	mbSupressCollectionsOpened = false;
}

bool CRG_PartShards::IsCollectionsOpen() const {
	if (!IsCreatureGame()) { return false; }
	auto window = WindowManager.GetMainWindow();
	auto partcollection = window->FindWindowByID(0x05E95446);
	
	if (partcollection && partcollection->IsVisible()) {
		return true;
	}	
	return false;
}

bool CRG_PartShards::ArePartsSelected() const {
	if (IsCollectionsOpen()) {
		return mSelectedParts.size() > 0;
	}
	return false;
}

void CRG_PartShards::SetPartSelectionType(int selectiontype) {
	if (mSelectionType != selectiontype) {
		ClearSelectedParts();
		mSelectionType = selectiontype;
		UpdateGambleButtonState();
		
	}
}

// take into account part tier, part type, etc
int CRG_PartShards::GetPartShardValue(ResourceKey partkey) const {
	if (IsPartCellPart(partkey)) { // || IsPartDetail(partkey)
		if (IsPartMouth(partkey)) {
			return mPartShardValue;
		}
		return mPartShardValueCell;
	}
	else {
		int parttier_mult = GetPartUnlockLevel(partkey); // 1-4
		int parttier_bonus = (parttier_mult-1) * mPartShardValuePerTier; // add mPartShardValuePerTier for every level over 1

		int value = mPartShardValue + parttier_bonus;

		if (IsPartMouth(partkey) || IsPartWing(partkey)) {
			return (value + (mPartShardValuePerTier * 2)); // add double extra bonus for mouths
		}
		else if (IsPartHandFootWeapon(partkey)) {
			return (value + mPartShardValuePerTier); // add extra bonus for hands, feet, and weapons
		}
		return (value);
	}
}

// tkae into account part tier, part type, etc
int CRG_PartShards::GetPartShardBuybackCost(ResourceKey partkey) const {
	if (IsPartCellPart(partkey)) { // || IsPartDetail(partkey)
		if (IsPartMouth(partkey)) {
			return mPartShardCost;
		}
		return mPartShardCostCell;
	}
	else {
		int parttier_mult = GetPartUnlockLevel(partkey); // 1-4
		int parttier_bonus = (parttier_mult - 1) * mPartShardCostPerTier; // add mPartShardValuePerTier for every level over 1

		int value = mPartShardCost + parttier_bonus;

		if (IsPartMouth(partkey) || IsPartWing(partkey)) {
			return (value + (mPartShardCostPerTier + mPartShardValuePerTier)); // add double extra bonus for mouths
		}
		else if (IsPartHandFootWeapon(partkey)) {
			return (value + mPartShardCostPerTier); // add extra bonus for hands, feet, and weapons
		}
		return (value);
	}
}

int CRG_PartShards::GetAllPartsShardValue() const {
	int value = 0;
	for (size_t i = 0; i < mSelectedParts.size(); i++) {
		value += GetPartShardValue(mSelectedParts[i]);
	}
	return value;
}

int CRG_PartShards::GetAllPartsShardBuybackCost() const {
	int value = 0;
	for (size_t i = 0; i < mSelectedParts.size(); i++) {
		value += GetPartShardBuybackCost(mSelectedParts[i]);
	}
	return value;
}

ResourceKey CRG_PartShards::GetPartKeyFromWindow(IWindow* window) const {
	ResourceKey partKey;
	auto image = window->FindWindowByID(0x060B1040);
	// if image is not valid, the window is offscreen. so, we might want to store this when it is first clicked.
	if (image) {
		auto imageID = window->FindWindowByID(0x060B1041)->GetCommandID();
		partKey = GetPartKeyFromID(imageID);
	}

	return partKey;
}

ResourceKey CRG_PartShards::GetPartKeyFromID(uint32_t imageID) const {
	ResourceKey partKey;
	for (cCollectableItemID item : Simulator::GetPlayer()->mpCRGItems->mUnlockedItems) {
		// found a match
		if (item.instanceID == imageID) {
			partKey.groupID = item.groupID;
			partKey.instanceID = item.instanceID;
			break;
		}
	}

	return partKey;
}

// Mouths

// are all remaining mouth parts selected?
bool CRG_PartShards::AreAllMouthsSelected() const {
	if (IsCollectionsOpen()) {
		if (mMouthParts.size() == 0 || mMouthsSelected == 0) { return false; }

		return (mMouthParts.size() == mMouthsSelected + mMouthsDestroyed);
	}
	return false;
}


//---------------------------------------------
// Part Windows

bool CRG_PartShards::IsPartWindowValid(IWindow* window) {
	auto questionmark = window->FindWindowByID(0x060B0F50);
	if (questionmark && questionmark->IsVisible()) {
		return false;
	}
	return true;
}

bool CRG_PartShards::IsPartWindowUnlocked(IWindow* window) {
	if (IsPartWindowValid(window)) {
		auto locked = window->FindWindowByID(0x060B0F51);
		if (locked && locked->IsVisible()) {
			return false;
		}
		return true;
	}
	return false;
}

bool CRG_PartShards::IsPartWindowInUse(IWindow* window) {
	if (IsPartWindowValid(window)) {
		auto partused = window->FindWindowByID(0x060B0F52);
		if (partused && partused->IsVisible()) {
			return true;
		}
		return false;
	}
	return false;
}

bool CRG_PartShards::IsPartWindowSelected(IWindow* window) {

	auto highlight = window->FindWindowByID(0x060B0E45);
	return highlight->IsVisible();
}

void CRG_PartShards::SetPartWindowSelected(IWindow* window, bool selected) {
	SetGambleButtonState(Redeem);
	if (IsCollectionsOpen()) {
		auto highlight = window->FindWindowByID(0x060B0E45);
		if (highlight) {
			highlight->SetVisible(selected);
		}
		ResourceKey partKey = GetPartKeyFromWindow(window);
		
		if (selected) {
			mSelectedParts.push_back(partKey);

			mSelectedPartWindows.push_back(window);
			if (IsPartMouth(GetPartKeyFromWindow(window))) {
				mMouthsSelected++;
			}
		}
		else {
			if (mSelectedParts.size() > 0) {
				// reform part vector without this key.
				vector<ResourceKey> mSelectedPartsNew;
				for (size_t i = 0; i < mSelectedParts.size(); i++) {
					if (mSelectedParts[i] != partKey) {
						mSelectedPartsNew.push_back(mSelectedParts[i]);
					}
				}
				mSelectedParts = mSelectedPartsNew;

				// reform window vector without this window.
				vector<IWindow*> mSelectedPartWindowsNew;
				for (size_t i = 0; i < mSelectedPartWindows.size(); i++) {
					if (mSelectedPartWindows[i] != window) {
						mSelectedPartWindowsNew.push_back(mSelectedPartWindows[i]);
					}
				}
				mSelectedPartWindows = mSelectedPartWindowsNew;

				if (IsPartMouth(GetPartKeyFromWindow(window))) {
					mMouthsSelected--;
				}
			}
		}
		UpdatePartValue();
	}
}

// Modify the part rollover cost and value.
void CRG_PartShards::PartWindowHovered(IWindow* window) {
	if (!window) { return; }
	auto partrollover = WindowManager.GetMainWindow()->FindWindowByID(0x05011AD2);
	if (partrollover && partrollover->IsVisible()) {
		ResourceKey partKey = GetPartKeyFromWindow(window);
		if (partKey.instanceID == 0x0) { return; }
		auto cost = partrollover->FindWindowByID(0x023288A3);

		eastl::string16 pricestr;
		int pricevalue = 0;

		if (IsPartWindowUnlocked(window)) { pricevalue = GetPartShardValue(partKey); }
		else { pricevalue = GetPartShardBuybackCost(partKey); }
		pricestr.sprintf(u"▲%i", pricevalue);

		cost->SetCaption(pricestr.c_str()); //⚌ //◭
	}
}

void CRG_PartShards::SetPartWindowUnlocked(IWindow* window, bool state) {
	// NOTE: these sub-windows will be invalid for windows that are on other pages.
	if (window) {
		auto highlight = window->FindWindowByID(0x060B0E45);
		auto star = window->FindWindowByID(0x060B18D0);
		auto hover = window->FindWindowByID(0x060B0E48);
		auto image = window->FindWindowByID(0x060B1040);
		auto question = window->FindWindowByID(0x060B0F50);
		auto locked = window->FindWindowByID(0x060B0F51);
		auto partused = window->FindWindowByID(0x060B0F52);

		if (highlight) {
			// Enabled
			if (state) {
				highlight->SetVisible(false);
				star->SetVisible(false);
				hover->SetVisible(false);
				image->SetShadeColor(Color(0xFFFFFFFF));
				question->SetVisible(false);
				locked->SetVisible(false);
				// Part used on creature
				if (GameNounManager.GetAvatar()) {
					bool ispartused = IsPartUsedOnCreature(GetPartKeyFromWindow(window), GameNounManager.GetAvatar());
					partused->SetVisible(ispartused);
					if (ispartused) {
						image->SetShadeColor(mColorPartInUse.ToIntColor());
						auto tooltip = CreateTooltip(localePartInUse.GetText());
						window->AddWinProc(tooltip);
					}
				}
			}
			// Disabled
			else {
				highlight->SetVisible(false);
				star->SetVisible(false);
				hover->SetVisible(false);
				image->SetShadeColor(mColorPartDisabled.ToIntColor());
				question->SetVisible(false);
				locked->SetVisible(true);
			}
			
		}
	}
	
}

Tooltip* CRG_PartShards::GetWindowTooltip(IWindow* window) const {
	int tries = 16;
	IWinProc* last_winproc = nullptr;
	while (tries > 0) {
		tries--;
		last_winproc = window->GetNextWinProc(last_winproc);
		if (last_winproc == nullptr) { // end if none left
			return nullptr;
		}
		Tooltip* tooltip = object_cast<Tooltip>(last_winproc);
		if (tooltip) {
			return tooltip;
		}
	}
	return nullptr;
}


IGlideEffect* CRG_PartShards::GetWindowGlide(IWindow* window) const {
	int tries = 16;
	IWinProc* last_winproc = nullptr;
	while (tries > 0) {
		tries--;
		last_winproc = window->GetNextWinProc(last_winproc);
		if (last_winproc == nullptr) { // end if none left
			return nullptr;
		}
		IGlideEffect* glide = object_cast<IGlideEffect>(last_winproc);
		if (glide) {
			return glide;
		}
	}
	return nullptr;
}

IInflateEffect* CRG_PartShards::GetWindowInflate(IWindow* window) const {
	int tries = 16;
	IWinProc* last_winproc = nullptr;
	while (tries > 0) {
		tries--;
		last_winproc = window->GetNextWinProc(last_winproc);
		if (last_winproc == nullptr) { // end if none left
			return nullptr;
		}
		IInflateEffect* inflate = object_cast<IInflateEffect>(last_winproc);
		if (inflate) {
			return inflate;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------------
// UI messages
#include <Spore\Editors\Editor.h>
using namespace Editors;

bool CRG_PartShards::IsPaletteItemDestroyed(StandardItemUIPtr item) const {
	auto itemKey = item->mpItem->mName;
	return IsPartDestroyed(itemKey);
}

void CRG_PartShards::ProcessPaletteItem(StandardItemUIPtr item) const {
	if (item && IsPaletteItemDestroyed(item)) {

		item->mpWindow->SetEnabled(false);
		item->mpWindow->SetIgnoreMouse(true);
		item->mpWindow->SetCursorID(0x0);
		item->mpWindow->SetShadeColor(0x0);
		auto drawable = object_cast<UTFWin::IImageDrawable>(item->mpWindow->GetDrawable());
		drawable->SetImage(nullptr);

		ResourceKey icon = { item->mpItem->mName.instanceID, TypeIDs::Names::png, 0x02231C8B };
		auto panel = IImageDrawable::AddImageWindow(icon, 0, 0, item->mpWindow.get());
		panel->SetSize(item->mpWindow->GetArea().GetWidth(), item->mpWindow->GetArea().GetHeight());
		panel->SetShadeColor(mColorPartPaletteDisabled.ToIntColor());

	}
}

void CRG_PartShards::SuppressDestroyedParts() {

	if (mDestroyedParts.size() == 0) { return; }
	if (!(GetEditor() && GetEditor()->IsActive())) { return; }
	if (GetEditor()->mSaveExtension != 0x2B978C46) { return; }

	PaletteUIPtr palette = GetEditor()->mpPartsPaletteUI;
	if (!palette) { return; }

	int itemcount = 0;
	for (PaletteCategoryUIPtr catUI : palette->mCategories) {
		PaletteSubcategoriesUIPtr subCatUIs = catUI->mpSubcategoriesUI;

		// subcategories present
		if (subCatUIs) {
			for (PaletteCategoryUIPtr subCatUI : subCatUIs->mCategoryUIs) {
				for (auto pageUI : subCatUI->mPageUIs) {
					// standard editor/planner
					if (pageUI.page->mStandardItems.size() > 0) {
						for (StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
							ProcessPaletteItem(itemUI);
							itemcount++;
						}
					}

				}
			}
		}
		// simple category
		else {
			for (auto pageUI : catUI->mPageUIs) {
				for (StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
					ProcessPaletteItem(itemUI);
					itemcount++;
				}
			}
		}
		
	}
	itemcount = itemcount;
}

//-----------------------------------------------------------------------------------------
// UI messages

int CRG_PartShards::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced | kEventRefresh;
}

bool CRG_PartShards::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsCreatureGame()) { return false; }

	// Pressed buttosn
	if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();

		// Gamble button clicked
		if (id == 0x036AD4A4) {
			GambleButtonPressed();
		}
		
	}
	// Selected/Toggled button
	else if (message.IsType(kMsgButtonSelect)) {
		//auto id = message.source->GetControlID();
		//auto commandid = message.source->GetCommandID();
		auto button = object_cast<IButton>(message.source);

		// Part window clicked
		if (button && button->GetButtonGroupID() == 0x060B1490) {
			if (IsPartWindowInUse(message.source)) { return false; }
			// Selling unlocked parts
			else if (IsPartWindowUnlocked(message.source)) {
				SetPartSelectionType(Sell);
				SetPartWindowSelected(message.source, !IsPartWindowSelected(message.source));
			}
			// Buying back locked parts
			else if (IsPartWindowValid(message.source)) {
				SetPartSelectionType(Buy);
				SetPartWindowSelected(message.source, !IsPartWindowSelected(message.source));
				mMouthsSelected = 0;
			}
		}
	}
	// hovered button
	else if (message.IsType(kMsgMouseEnter)) {
		auto button = object_cast<IButton>(message.source);

		// Part window hovered
		if (button && button->GetButtonGroupID() == 0x060B1490) {
			PartWindowHovered(message.source);
		}
	}

	return false;
}