#include "stdafx.h"
#include "cPaletteUnlockStructs.h"
#include <Spore\UTFWin\InflateEffect.h>

///------------------------
/// PaletteUnlockData
///------------------------


// Store and return a list of palette category IDs from the mPartUnlockConfigID
vector<uint32_t> cPaletteUnlockStructs::PaletteUnlockData::GetUnlockCategories() {
	if (mPartUnlockCategories.size() > 0) return mPartUnlockCategories;

	if (mPartUnlockConfigID != 0x0) {
		// partUnlocks~
		uint32_t partUnlockFeedList = CapabilityChecker::GetModelUInt32Value({mPartUnlockConfigID, prop, 0x7b2c1325}, 0x055DD9F6);
		if (partUnlockFeedList != 0x0) {
			// AssetBrowserFeedList
			vector<ResourceKey> feedListCategories = CapabilityChecker::GetModelKeyValues({partUnlockFeedList, prop, 0x6edc12d4}, 0x744717C0);
			for (size_t i = 0; i < feedListCategories.size(); i++) {
				// AssetBrowserFeedCategories
				vector<ResourceKey> feedListItems = CapabilityChecker::GetModelKeyValues({feedListCategories[i].instanceID, prop, 0x06b38241}, 0x744717C1);
				for (size_t j = 0; j < feedListItems.size(); j++) {
					// AssetBrowserFeedItems
					uint32_t feedListItemUniqueID = CapabilityChecker::GetModelUInt32Value({feedListItems[j].instanceID, prop, 0x4e5892eb}, 0x06678DF3);
					mPartUnlockCategories.push_back(feedListItemUniqueID);
				}
			}
		}
	}
	return mPartUnlockCategories;
}

// items added with default params will not unlock in any specific category/page/coords
void cPaletteUnlockStructs::PaletteUnlockData::AddUnlockableItem(PaletteItemPtr item, uint32_t categoryID, int pageIndex, Vector2 coords) {
	if (!item) return;
	UnlockableItem itemstruct = CreateUnlockableItemFromKey(item->mName);
	if (itemstruct.mKey.instanceID) {
		mUnlockableItems[item->mName] = itemstruct;
	}
	mUnlockableItems[item->mName] = itemstruct;
}

cPaletteUnlockStructs::UnlockableItem cPaletteUnlockStructs::PaletteUnlockData::CreateUnlockableItemFromKey(const ResourceKey& key, uint32_t categoryID, int pageIndex, Vector2 coords, bool usefindpercent) {
	if (!key.instanceID) return {};
	float itemUnlockFindPercentage = CapabilityChecker::GetModelFloatValue(key, 0x03A289C3);
	if (itemUnlockFindPercentage == 0) return {};
	uint32_t itemUnlockEffect = CapabilityChecker::GetModelKeyInstanceID(key, 0xD3BCDC94);
	if (itemUnlockEffect == 0) {
		itemUnlockEffect = UnlockableItem::kUnlockEffectID;
	}
	UnlockableItem itemstruct = {
		/* ResourceKey mKey */ key,
		/* int itemUnlockLevel */	CapabilityChecker::GetModelIntValue(key, 0x03A289AC),
		/* float itemUnlockFindPercentage */ CapabilityChecker::GetModelFloatValue(key, 0x03A289C3),
		/* uint32_t categoryID */ categoryID,
		/* int column */	(int)coords.x,
		/* int row */		(int)coords.y,
		/* int pageIndex */ pageIndex,
		/* uint32_t itemUnlockEffect */ itemUnlockEffect,
		/* rigblocktype */ CapabilityChecker::GetModelKeyInstanceID(key, 0x0186609D) // modelRigBlockType
	};
	return itemstruct;
}

///------------------------------------------
/// Interface with Items

// Return an UnlockableItem* struct pointer from mUnlockedItems
cPaletteUnlockStructs::UnlockedItem* cPaletteUnlockStructs::PaletteUnlockData::GetUnlockedItem(ResourceKey key) {
	for (size_t i = 0; i < mUnlockedItems.size(); i++) {
		if ((mUnlockedItems[i].mKey.instanceID == key.instanceID || mUnlockedItems[i].mKeySymmetric.instanceID == key.instanceID)
			&& mUnlockedItems[i].mKey.groupID == key.groupID) {
			return &mUnlockedItems[i];
		}
	}
	return nullptr;
}

// Return an UnlockableItem* data struct pointer from mUnlockableItems
// Also accepts a resourcekey from a symmetric variant
cPaletteUnlockStructs::UnlockableItem* cPaletteUnlockStructs::PaletteUnlockData::GetUnlockableItem(ResourceKey key) {
	auto item = &mUnlockableItems[GetUnlockablePartSymVariant(key)];
	if (item && item->mKey.instanceID) {
		return item;
	}
	else {
		item->Set(CreateUnlockableItemFromKey(key));
		return item;
	}
}

// Return the symmetric variant of a part that is in mUnlockableItems
ResourceKey cPaletteUnlockStructs::PaletteUnlockData::GetUnlockablePartSymVariant(ResourceKey key) {
	// Check for whatever part is given
	auto item = &mUnlockableItems[key];
	if (item && item->mKey.instanceID) {
		return key;
	}
	else {
		// Check for symmetric part RIGHT
		ResourceKey sym = CapabilityChecker::GetSymmetricVariant(key, CapabilityChecker::RIGHT);
		if (sym.instanceID != key.instanceID) {
			if (&mUnlockableItems[sym] && &mUnlockableItems[sym].mKey.instanceID) {
				return sym;
			}
			
		}
		// Check for symmetric part LEFT (used if given key is a right variant and palette item is left)
		else {
			ResourceKey sym = CapabilityChecker::GetSymmetricVariant(key, CapabilityChecker::LEFT);
			if (sym.instanceID != key.instanceID) {
				if (&mUnlockableItems[sym] && &mUnlockableItems[sym].mKey.instanceID) {
					return sym;
				}
			}
		}
		// Not found, return existing key
		return key;
	}
}

// Check if the list contains an UnlockableItem with this key
bool cPaletteUnlockStructs::PaletteUnlockData::IsItemUnlocked(ResourceKey key) {
	return bool(GetUnlockedItem(key));
}

// Turn a resource key into an UnlockableItem and add it to the list
// TODO: should this return the created UnlockableItem?
void cPaletteUnlockStructs::PaletteUnlockData::Unlock(ResourceKey key, bool highlight, bool update_collection) {
	key.typeID = prop;
	auto unlockable = GetUnlockableItem(key);
	key = unlockable->mKey;
	if (!IsItemUnlocked(key)) {
		mUnlockedItems.push_back(UnlockedItem(key));
		// update collection with new item
		if (update_collection && GetCollection()) {
			GetCollection()->mUnlockedItems.push_back({ key.instanceID, key.groupID });
		}
		if (highlight) {
			PlayUnlockEffect(unlockable->itemUnlockEffect);
		}
		else {
			// if unset, disable highlight.
			//UnlockedItem* item = GetUnlockedItem(key);
			//if (item) item->mbNewItem = false;
		}
	}
	
}

// Unlock all unlockable parts
void cPaletteUnlockStructs::PaletteUnlockData::UnlockAll() {
	mUnlockedItems.clear();
	auto it = mUnlockableItems.begin();
	while (it != mUnlockableItems.end()) {
		UnlockedItem(it.get_node()->mValue.first);
		GetCollection()->mUnlockedItems.push_back({ it.get_node()->mValue.first.instanceID, it.get_node()->mValue.first.groupID });
	}
}

void cPaletteUnlockStructs::PaletteUnlockData::ClearUnlocks() {
	mUnlockedItems.clear();
	GetCollection()->mUnlockedItems.clear();
}

// Set mbNewItem on every item to false
void cPaletteUnlockStructs::PaletteUnlockData::MarkItemsRead() {
	for (size_t i = 0; i < mUnlockedItems.size(); i++) {
		mUnlockedItems[i].mbNewItem = false;
	}
}

///------------------------------------------
/// Interface with Collections

cCollectableItemsPtr cPaletteUnlockStructs::PaletteUnlockData::GetCollection() const {
	switch (mCollectionID) {
	case CLG: return StarManager.mpGlobalCLGItems; // Cell Parts
	case CRG: return GetPlayer()->mpCRGItems; // Creature Parts
	}
	return nullptr;
}

// Adds items from a collection into the unlock data
void cPaletteUnlockStructs::PaletteUnlockData::PopulateFromCreation(ResourceKey& creation, bool associated_parts)
{
	auto unlocks = AssociatedPartUnlocks(creation, associated_parts);
	auto it = unlocks.mUnlockedParts.begin();
	while (it != unlocks.mUnlockedParts.end()) {
		Unlock(ResourceKey(it->instanceID, prop, it->groupID), false, false);
		it = it.next();
	}
}

// Adds items from the associated collection into the unlock data
void cPaletteUnlockStructs::PaletteUnlockData::PopulateFromCollection()
{
	auto collection = GetCollection();
	if (!collection) return;
	for (auto item : collection->mUnlockedItems) {
		Unlock(ResourceKey(item.instanceID, prop, item.groupID), false, false);
	}
}

// MOVED TO cPaletteUnlock.cpp
//void cPaletteUnlockStructs::PaletteUnlockData::PopulatePaletteParts() {}

// Adds items from mUnlockedItems into Collections
void cPaletteUnlockStructs::PaletteUnlockData::UpdateCollection()
{
	auto collection = GetCollection();
	if (!collection) return;
	collection->mUnlockedItems.clear();
	for (size_t i = 0; i < mUnlockedItems.size(); i++) {
		collection->mUnlockedItems.push_back({ mUnlockedItems[i].mKey.instanceID, mUnlockedItems[i].mKey.groupID });
	}
}

void cPaletteUnlockStructs::PaletteUnlockData::PlayUnlockEffect(uint32_t fx) {
	if (!IsStageGameMode()) return;
	IVisualEffectPtr effect;
	if (EffectsManager.CreateVisualEffect(fx, 0, effect)) {
		Vector3 pos = {0,0,0};
		float height = 0.5;
		Quaternion rot = {0,0,0,0};
		if (IsCreatureGame()) {
			pos = GameNounManager.GetAvatar()->GetPosition();
			height += GameNounManager.GetAvatar()->GetModel()->mDefaultBBox.upper.z;
			rot = GameNounManager.GetAvatar()->GetOrientation();
		} else if (IsCellGame()) {
			pos = Cell::GetPlayerCell()->GetPosition();
			rot = Cell::GetPlayerCell()->mTransform.GetRotation().ToQuaternion();
		}
		pos = pos + (height * pos.Normalized());

		Transform trans = effect->GetRigidTransform();
		trans.SetOffset(pos);
		trans.SetRotation(rot);
		trans.SetScale(1.0f);
		effect->SetRigidTransform(trans);
		effect->Start(0);
	}
}

///------------------------
/// AssociatedPartUnlocks
///------------------------

// TODO: patch this with a fix for palette entries of symmetric parts?
cPaletteUnlockStructs::AssociatedPartUnlocks::AssociatedPartUnlocks(ResourceKey& creation, bool associated_parts) {

	// Loop through all parts on the creature.
	vector<Editors::cEditorResourceBlock> parts = CapabilityChecker::GetCreationRigblocks(creation);
	bool loaded_vertebra = false;
	for (size_t n = 0; n < parts.size(); n++) {
		ResourceKey part = { parts[n].instanceID, prop, parts[n].groupID };

		// convert symmetric variant to left variant
		//part = CapabilityChecker::GetSymmetricVariant(part);
		// Only load vertebra once.
		bool is_spine = CapabilityChecker::GetModelIntValue(part, 0x100F0F5A) > 0;
		bool is_limb = CapabilityChecker::GetModelIntValue(part, 0xB00F0FDF) > 0 || CapabilityChecker::GetModelIntValue(part, 0x04DBCDD2) > 0;
		if (is_spine) {
			if (!loaded_vertebra) loaded_vertebra = true;
			else continue;
		}

		hash_map<int, vector<ResourceKey>> unlocks;
		unlocks[0] = CapabilityChecker::GetModelKeyValues(part, 0x046BF76C); // associatedPartUnlocks
		if (associated_parts) {
			unlocks[1] = CapabilityChecker::GetModelKeyValues(part, 0x052C6CDA); // associatedRandomPartUnlocks
			unlocks[2] = CapabilityChecker::GetModelKeyValues(part, 0x052C6CDB); // associatedRandomPartUnlocks2
			unlocks[3] = CapabilityChecker::GetModelKeyValues(part, 0x052C6CDC); // associatedRandomPartUnlocks3
			unlocks[4] = CapabilityChecker::GetModelKeyValues(part, 0x052C6CDD); // associatedRandomPartUnlocks4
			unlocks[5] = CapabilityChecker::GetModelKeyValues(part, 0x052C6CDE); // associatedRandomPartUnlocks5
		}
		// How many parts to randomly unlock from each list (unlocks[0] will always unlock all)
		hash_map<int, int> unlocksAmt;
		unlocksAmt[0] = -1;
		if (associated_parts) {
			unlocksAmt[1] = CapabilityChecker::GetModelIntValue(part, 0x052C6CC0); // numPartsToRandomlyUnlock
			unlocksAmt[2] = CapabilityChecker::GetModelIntValue(part, 0x052C6CC1); // numPartsToRandomlyUnlock2
			unlocksAmt[3] = CapabilityChecker::GetModelIntValue(part, 0x052C6CC2); // numPartsToRandomlyUnlock3
			unlocksAmt[4] = CapabilityChecker::GetModelIntValue(part, 0x052C6CC3); // numPartsToRandomlyUnlock4
			unlocksAmt[5] = CapabilityChecker::GetModelIntValue(part, 0x052C6CC4); // numPartsToRandomlyUnlock5
		}

		// Unlock parts from the above data.

		if (!is_spine && !is_limb) {
			// Always unlock the part itself
			mUnlockedParts.emplace_back(part);
		}

		for (size_t i = 0; i < (size_t)5; i++) {
			// Unlock all from list 0
			if (i == 0) {
				for (size_t j = 0; j < unlocks[i].size(); j++) {
					mUnlockedParts.emplace_back(unlocks[i][j]);
				}
			}
			// Unlock randomly from lists 1-5
			else {
				int amt = unlocksAmt[i];
				if (amt > 0 && unlocks[i].size() > 0) {
					amt = clamp(amt, 0, (int)unlocks[i].size());

					int used_idxs[8] = {};
					// Unlock 'amt' random items from this list
					for (size_t j = 0; j <= (size_t)amt; j++) {

						// Ensure the random indexes are not being re-used
						bool used_idx = true; // has this index been used?
						size_t rand_idx = rand(unlocks[i].size());
						while (used_idx) {
							used_idx = false;
							for (size_t k = 0; k < 8; k++) {
								if (used_idxs[k] == rand_idx) {
									used_idx = true;
									break;
								}
							}
							// index has been used, reroll
							if (used_idx) {
								rand_idx = rand(unlocks[i].size());
							}
						}

						mUnlockedParts.emplace_back(unlocks[i][rand_idx]);
					}
				}
			}
		}
		mUnlockedParts.unique();
	}
}

///------------------------
/// cPaletteUnlockStructs
///------------------------

// Get all palette items from a palette
vector<PaletteItemPtr> cPaletteUnlockStructs::GetPaletteItems(const PaletteMainPtr palette) {

	vector<PaletteItemPtr> items = {};
	if (!palette) { return items; }

	// Loop through all categories, subcategories, and pages to find palette items.
	for (const PaletteCategoryPtr cat : palette->mCategories) {
		eastl::vector<PaletteCategoryPtr> subCats = cat->mChildren;
		if (subCats.size() == 0) {
			subCats.push_back(cat);
		}

		for (const PaletteCategoryPtr subCat : subCats) {
			for (const auto page : subCat->mPages) {
				// standard editor/planner
				if (page->mItems.size() > 0) {
					for (const PaletteItemPtr item : page->mItems) {
						if (item) items.push_back(item);
					}
				}
			}
		}
	}
	return items;
}

// Get all palette item UIs from the current editor
vector<StandardItemUIPtr> cPaletteUnlockStructs::GetPaletteItemUIs(const PaletteUIPtr paletteUI) { //  vector<uint32_t> categories

	vector<StandardItemUIPtr> items = {};
	if (!paletteUI) { return items; }

	// Loop through all categories, subcategories, and pages to find palette items.
	for (const PaletteCategoryUIPtr catUI : paletteUI->mCategories) {
		vector<PaletteCategoryUIPtr> subCatUIs;
		if (catUI->mpSubcategoriesUI) {
			subCatUIs = catUI->mpSubcategoriesUI->mCategoryUIs;
		}
		if (subCatUIs.size() == 0) {
			subCatUIs.push_back(catUI);
		}
		for (const PaletteCategoryUIPtr subCatUI : subCatUIs) {
			for (const auto pageUI : subCatUI->mPageUIs) {
				// standard editor/planner
				if (pageUI.page->mStandardItems.size() > 0) {
					for (const StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
						items.push_back(itemUI);
					}
				}
			}
		}
	}
	return items;
}

vector<PaletteItemPtr> cPaletteUnlockStructs::GetCategoryItems(const PaletteCategoryPtr category) {

	vector<PaletteItemPtr> items = {};
	if (!category) { return items; }

	// Loop through all subcategories and pages to find palette items.
	eastl::vector<PaletteCategoryPtr> subCats = category->mChildren;
	if (subCats.size() == 0) {
		subCats.push_back(category);
	}

	for (const PaletteCategoryPtr subCat : subCats) {
		for (const auto page : subCat->mPages) {
			// standard editor/planner
			if (page->mItems.size() > 0) {
				for (const PaletteItemPtr item : page->mItems) {
					if (item) items.push_back(item);
				}
			}
		}
	}
	return items;
}

vector<StandardItemUIPtr> cPaletteUnlockStructs::GetCategoryItemUIs(const PaletteCategoryUIPtr categoryUI) {

	vector<StandardItemUIPtr> items = {};
	if (!categoryUI) { return items; }

	// Loop through all subcategories and pages to find palette items.
	vector<PaletteCategoryUIPtr> subCatUIs;
	if (categoryUI->mpSubcategoriesUI) {
		subCatUIs = categoryUI->mpSubcategoriesUI->mCategoryUIs;
	}
	if (subCatUIs.size() == 0) {
		subCatUIs.push_back(categoryUI);
	}
	for (const PaletteCategoryUIPtr subCatUI : subCatUIs) {
		for (const auto pageUI : subCatUI->mPageUIs) {
			// standard editor/planner
			if (pageUI.page->mStandardItems.size() > 0) {
				for (const StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
					items.push_back(itemUI);
				}
			}
		}
	}
	return items;
}

vector<PaletteItemPtr> cPaletteUnlockStructs::GetPageItems(const PalettePagePtr page) {
	vector<PaletteItemPtr> items = {};
	if (!page) { return items; }
	if (page->mItems.size() > 0) {
		for (const PaletteItemPtr item : page->mItems) {
			if (item) items.push_back(item);
		}
	}
	return items;
}

vector<StandardItemUIPtr> cPaletteUnlockStructs::GetPageItemUIs(const PalettePageUIPtr pageUI) {
	vector<StandardItemUIPtr> items = {};
	if (!pageUI) { return items; }
	if (pageUI->mStandardItems.size() > 0) {
		for (const StandardItemUIPtr itemUI : pageUI->mStandardItems) {
			items.push_back(itemUI);
		}
	}
	return items;
}

//-------------------------------------------------------------------------

bool cPaletteUnlockStructs::IsItemHidden(const StandardItemUIPtr item) {
	return !item->mpWindow->IsEnabled();
}

void cPaletteUnlockStructs::ItemHide(const StandardItemUIPtr item)
{
	item->mpWindow->SetEnabled(false);
	item->mpWindow->SetIgnoreMouse(true);
	item->mpWindow->SetCursorID(0x0);
	const auto drawable = object_cast<UTFWin::IImageDrawable>(item->mpWindow->GetDrawable());
	if (drawable) {
		drawable->SetImage(nullptr);
	}
}

// Unhide the palette item
void cPaletteUnlockStructs::ItemShow(const StandardItemUIPtr item)
{
	item->mpWindow->SetEnabled(true);
	item->mpWindow->SetIgnoreMouse(false);
	item->mpWindow->SetCursorID(0x648fbf1);
	const auto drawable = object_cast<UTFWin::IImageDrawable>(item->mpWindow->GetDrawable());
	if (drawable) {
		ImagePtr image;
		Image::GetImage(item->mpItem->mThumbnailName, image);
		drawable->SetImage(image.get());
	}
}

// Create an image panel for the palette item. If a panel already exists, use that.
IWindow* cPaletteUnlockStructs::ItemCreateImgPanel(const StandardItemUIPtr item, const ResourceKey& icon, IWindow* parent)
{
	if (!parent) {
		parent = item->mpWindow.get();
	}
	IWindow* panel = IImageDrawable::AddImageWindow(icon, 0, 0, parent);
	panel->SetControlID(kImgPanelID);
	return panel;
}

// Find if there is an existing image panel with controlID kImgPanelID and remove it.
// Also remove this item's parent's image panels that match the item's instance ID (new part highlight)
void cPaletteUnlockStructs::ItemRemoveImgPanel(const StandardItemUIPtr item)
{
	auto panel = item->mpWindow->FindWindowByID(kImgPanelID, false);
	while (panel) {
		item->mpWindow->RemoveWindow(panel);
		panel = item->mpWindow->FindWindowByID(kImgPanelID, false);
	}
	panel = item->mpWindow->GetParent()->FindWindowByID(item->mpItem->mName.instanceID, false);
	while (panel) {
		item->mpWindow->GetParent()->RemoveWindow(panel);
		panel = item->mpWindow->GetParent()->FindWindowByID(item->mpItem->mName.instanceID, false);
	}
}

// Gray out an unlocked item
void cPaletteUnlockStructs::ItemSetDisabled(const StandardItemUIPtr item)
{
	if (IsItemHidden(item)) return;
	ItemHide(item);
	ItemRemoveImgPanel(item);

	// Create icon window of blacked out part icon
	ResourceKey icon = { item->mpItem->mName.instanceID, png, 0x02231C8B };
	auto panel = ItemCreateImgPanel(item, icon);
	panel->SetSize(item->mpWindow->GetArea().GetWidth(), item->mpWindow->GetArea().GetHeight());
	panel->SetShadeColor(mColorPaletteItemDisabled.ToIntColor());
	panel->SetIgnoreMouse(false);
}

void cPaletteUnlockStructs::ItemSetLocked(const StandardItemUIPtr item)
{
	if (IsItemHidden(item)) return;
	ItemHide(item);
	ItemRemoveImgPanel(item);

	// Create icon window of unlockable shield
	const ResourceKey icon = { 0xC385fb5d, png, kGraphicsAtlas };
	auto panel = ItemCreateImgPanel(item, icon);
	object_cast<IImageDrawable>(panel->GetDrawable())->GetImage()->SetTexCoords(Math::Rectangle(0, 0, 0.84375, 0.546875));
	panel->AddWinProc(new ProportionalLayout(0.5, 0.5, 0.5, 0.5));
	panel->SetSize(27, 35);
	panel->SetLocation(-14, -15);
	panel->SetIgnoreMouse(false);
}

// TODO: make this highlight animate when opening its PAGE for the first time.
void cPaletteUnlockStructs::ItemSetNewHighlight(const StandardItemUIPtr item)
{
	if (IsItemHidden(item)) ItemReset(item);
	else ItemRemoveImgPanel(item);

	// Create backing window of star highlight
	const ResourceKey icon = { 0x90b45fcc, png, kGraphicsAtlas };
	auto panel = ItemCreateImgPanel(item, icon, item->mpWindow->GetParent());
	item->mpWindow->GetParent()->SendToBack(panel);
	object_cast<IImageDrawable>(panel->GetDrawable())->GetImage()->SetTexCoords(Math::Rectangle(0, 0, 0.65625, 0.328125));
	float sizescale = 0.88f;
	panel->SetSize(item->mpWindow->GetArea().GetWidth() * sizescale, item->mpWindow->GetArea().GetHeight() * sizescale);
	float halfoffset_x = (item->mpWindow->GetArea().GetWidth() - panel->GetArea().GetWidth()) / 2;
	float halfoffset_y = (item->mpWindow->GetArea().GetHeight() - panel->GetArea().GetHeight()) / 2;
	panel->SetLocation(item->mpWindow->GetArea().x1 + halfoffset_x, item->mpWindow->GetArea().y1 + halfoffset_x);
	panel->SetControlID(item->mpItem->mName.instanceID);

	// Set up animation for this highlight to play when enabled
	// (Make this panel visible to play the animation)
	panel->SetVisible(false);
	InflateEffect* inflate = new InflateEffect();
	inflate->SetScale(0.2f);
	inflate->SetInterpolationType(InterpolationType::DampedSpring);
	inflate->SetDamping(3.0f);
	inflate->SetEase(2, 2);
	inflate->SetTriggerType(TriggerType::Invisible);
	inflate->SetTime(8);

	panel->AddWinProc(inflate->ToWinProc());
	// TODO: move this to when the player first opens this page
	panel->SetVisible(true);
}

// Reset the item to its normal, unlocked state
void cPaletteUnlockStructs::ItemReset(const StandardItemUIPtr item)
{
	ItemShow(item);
	ItemRemoveImgPanel(item);
}