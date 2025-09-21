#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Editors\Editor.h>
#include "CapabilityChecker.h"

using namespace UTFWin;
using namespace UI;
using namespace Simulator;
using namespace Editors;
using namespace Palettes;
using namespace TypeIDs;

// Structs and static functions for interacting with palette unlocks

static const ColorRGBA mColorPaletteItemDisabled = { 0.04f, 0.048f, 0.066f, 0.65f };

class cPaletteUnlockStructs
{
public:
	//#define UnlockableItem cCollectableItems::UnlockableItem
	static const uint32_t kImgPanelID = 0x4DEDDD52;
	static const uint32_t kGraphicsAtlas = 0x31a44893;

	// based off cCollectableItems::UnlockableItem but with extra properties
	struct UnlockableItem
	{
		static const uint32_t kUnlockEffectID = id("CRG_partUnlock_limb");

		ResourceKey mKey = {}; // Key to a part, paint, tool, decor, or other palette item type to be unlocked.
		int itemUnlockLevel;
		float itemUnlockFindPercentage;
		uint32_t categoryID;
		int column;
		int row;
		int pageIndex;
		uint32_t itemUnlockEffect = kUnlockEffectID;
		uint32_t rigblocktype; // mouth, eye, ear, etc

		void Set(UnlockableItem item) {
			mKey = item.mKey;
			itemUnlockLevel = item.itemUnlockLevel;
			itemUnlockFindPercentage = item.itemUnlockFindPercentage;
			categoryID = item.categoryID;
			column = item.column;
			row = item.row;
			pageIndex = item.pageIndex;
			itemUnlockEffect = item.itemUnlockEffect;
			rigblocktype = item.rigblocktype;
		}
	};

	// Data struct for an unlocked item
	struct UnlockedItem {

		ResourceKey mKey = {}; // Key to a part, paint, tool, decor, or other palette item type to be unlocked.
		ResourceKey mKeySymmetric = {}; // Symmetric variant, to be unlocked in the palette with mKey if it exists.
		bool mbNewItem = true; // Has newly been unlocked, so FX can be played.
		bool mbHidden = false; // Black out this item in the palette

		UnlockedItem() {
		}

		UnlockedItem(ResourceKey key) {
			SetKey(key);
		}

		void SetKey(ResourceKey key) {
			mKey = key;
			mKeySymmetric = CapabilityChecker::GetSymmetricVariant(mKey, CapabilityChecker::RIGHT);
			if (mKeySymmetric == mKey) mKeySymmetric = {};
		}

		static Simulator::Attribute ATTRIBUTES[];
	};

	// Data struct for storing unlocked items
	struct PaletteUnlockData {
		// Base game collections that can be synced with unlocked items
		enum Collection {
			Null = 0,
			CLG = kGameCell,
			CRG = kGameCreature,
			TRG = kGameTribe, // Collection not in API yet
			ADV = kGameSpace, // Collection not in API yet
		};

		// Saved Data
		uint32_t mCollectionID = Null; // Collection to sync with
		vector<uint32_t> mEditorPaletteIDs = {}; // Unlock data will apply to these Palette IDs
		vector<UnlockedItem> mUnlockedItems = {}; // Items that have been unlocked, as UnlockedItem structs
		uint32_t mPartUnlockConfigID = {}; // Optional partUnlocks~ config to pull unlockable parts from. if unset, mPartUnlockCategories will not be filled.
		// TODO: not currently implemented
		//vector<uint32_t> mLimitToCategoryIDs = {}; // {} = Lock/Unlock will affect all categories. Otherwise, limit to these IDs and do not lock parts in excluded categories.

		// Figure out if I also want to allow unlocking categories themselves (gray if not unlocked) and if so, store those here too
		// Also figure this out for pages, if i want to have certain pages be hidden by default (eg Epic parts)

		// Runtime-populated Data
		vector<uint32_t> mPartUnlockCategories; // Palette category IDs to unlock parts from. if empty, unlock from all categories.
		hash_map<ResourceKey, UnlockableItem> mUnlockableItems; // Possible items that can be unlocked, as their keys mapped to UnlockableItem structs

		// TODO: maybe when all the parts in this row are unlocked, remove the row from this map?
		hash_map<cCollectableItemsRowID, cCollectableItemsRowVector> mUnlockableRows; // use with GetCollectableItemsRowID(uint32_t categoryID, int pageIndex, int row)


		PaletteUnlockData() {
		}
		PaletteUnlockData(uint32_t editorID, uint32_t unlockconfig = 0x0) {
			mEditorPaletteIDs.push_back(editorID);
			mPartUnlockConfigID = unlockconfig;
		}
		PaletteUnlockData(vector<uint32_t> editorIDs, uint32_t unlockconfig = 0x0) {
			mEditorPaletteIDs = editorIDs;
			mPartUnlockConfigID = unlockconfig;
		}

		// Populate the part unlocks with creation data, collection data, and (TODO) pre-unlocked parts.
		void PaletteUnlockData::Populate(ResourceKey species = {}) {
			if (species.instanceID) PopulateFromCreation(species);
			PopulateFromCollection();
			PopulatePaletteParts();
			UpdateCollection();
		}

		///------------------------------------------
		/// Interface with Unlock Config

		vector<uint32_t> GetUnlockCategories();
		void AddUnlockableItem(PaletteItemPtr item, uint32_t categoryID = 0x0, int pageIndex = 0, Vector2 coords = { 0,0 });
		UnlockableItem CreateUnlockableItemFromKey(const ResourceKey& key, uint32_t categoryID = 0x0, int pageIndex = 0, Vector2 coords = { 0,0 }, bool usefindpercent = false);

		///------------------------------------------
		/// Interface with Items

		UnlockedItem* GetUnlockedItem(ResourceKey key);
		UnlockableItem* GetUnlockableItem(ResourceKey key);
		ResourceKey GetUnlockablePartSymVariant(ResourceKey key);
		bool IsItemUnlocked(ResourceKey key);
		void Unlock(ResourceKey key, bool highlight = true, bool update_collection = true); // unlock a part directly
		void UnlockAll();
		void ClearUnlocks();
		ResourceKey GetRandPart(int level); // TODO: find the best next part to unlock
		ResourceKey GetNextPart(cCollectableItemsRowID rowID); // TODO: find the next unlockable part in this row
		void MarkItemsRead();

		///------------------------------------------
		/// Interface with Collections

		cCollectableItemsPtr GetCollection() const;
		void PopulateFromCreation(ResourceKey& creation, bool associated_parts = true);
		void PopulateFromCollection();
		void PopulatePaletteParts();
		void UpdateCollection();

		static Simulator::Attribute ATTRIBUTES[];

	private:
		void PlayUnlockEffect(uint32_t fx);
	};

	// Data struct that creates a list of parts to be unlocked on startup from that creature
	// Gethered from a creature's rigblock list.
	struct AssociatedPartUnlocks {
		// NOTE: all parts here will be their non-symmetric variants.
		list<ResourceKey> mUnlockedParts;

		AssociatedPartUnlocks(ResourceKey& creation, bool associated_parts = true);
	};

	//------------------------------------------------------------------------------------------

protected:
	///----------------------------------------------------------------
	/// Storing some general funcs here to declutter cPaletteUnlock...
	///----------------------------------------------------------------

	// Get Palette Items
	static vector<PaletteItemPtr> GetPaletteItems(const PaletteMainPtr paletteUI);
	static vector<StandardItemUIPtr> GetPaletteItemUIs(const PaletteUIPtr palette); //vector<uint32_t> categories = {} Optionally limit to these category IDs
	static vector<PaletteItemPtr> GetCategoryItems(const PaletteCategoryPtr category);
	static vector<StandardItemUIPtr> GetCategoryItemUIs(const PaletteCategoryUIPtr categoryUI);
	static vector<PaletteItemPtr> GetPageItems(const PalettePagePtr page);
	static vector<StandardItemUIPtr> GetPageItemUIs(const PalettePageUIPtr pageUI);

	// Palette Item Actions
	static bool IsItemHidden(const StandardItemUIPtr item);
	static void ItemHide(const StandardItemUIPtr item);
	static void ItemShow(const StandardItemUIPtr item);
	static IWindow* ItemCreateImgPanel(const StandardItemUIPtr item, const ResourceKey& icon, IWindow* parent = nullptr);
	static void ItemRemoveImgPanel(const StandardItemUIPtr item);
	//
	static void ItemSetDisabled(const StandardItemUIPtr item);
	static void ItemSetLocked(const StandardItemUIPtr item);
	static void ItemSetNewHighlight(const StandardItemUIPtr item);
	static void ItemReset(const StandardItemUIPtr item);

};
