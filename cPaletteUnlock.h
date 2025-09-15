#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Editors\Editor.h>
#include "CapabilityChecker.h"

#define cPaletteUnlockPtr intrusive_ptr<cPaletteUnlock>
#define PaletteUnlockManager (*cPaletteUnlock::Get())

using namespace UTFWin;
using namespace UI;
using namespace Simulator;
using namespace Editors;
using namespace Palettes;
using namespace TypeIDs;

class cPaletteUnlock
	: public Simulator::cStrategy
	, public App::IMessageListener
{
public:


	// Data struct for an unlockable item
	struct UnlockableItem {

		ResourceKey mKey = {}; // Key to a part, paint, tool, decor, or other palette item type to be unlocked.
		bool mbNewItem = true; // Has newly been unlocked, so FX can be played.
		bool mbHidden = false; // Black out this item in the palette

		UnlockableItem() {
		}

		UnlockableItem(ResourceKey key) {
			mKey = key;
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
			TRG = kGameTribe, // Not in API yet
			ADV = kGameSpace, // Not in API yet
		};

		uint32_t mCollectionID = Null; // Collection to sync with
		vector<uint32_t> mEditorPaletteIDs = {}; // Unlock data will apply to these Palette IDs
		vector<UnlockableItem> mUnlockedItems = {}; // Items that have been unlocked, as UnlockableItem structs
		uint32_t mPartUnlockConfigID = {}; // Optional partUnlocks~ config to pull unlockable parts from

		vector<uint32_t> mPartUnlockCategories; // Palette categories to unlock parts from. DO NOT SAVE, populated at runtime!
		// TODO: not currently implemented
		vector<uint32_t> mLimitToCategoryIDs = {}; // {} = Lock/Unlock will affect all categories. Otherwise, limit to these IDs and do not lock parts in excluded categories.

		// Figure out if I also want to allow unlocking categories themselves (gray if not unlocked) and if so, store those here too
		// Also figure this out for pages, if i want to have certain pages be hidden by default (eg Epic parts)

		PaletteUnlockData() {
		}
		PaletteUnlockData(uint32_t editorID) {
			mEditorPaletteIDs.push_back(editorID);
		}
		PaletteUnlockData(vector<uint32_t> editorIDs) {
			mEditorPaletteIDs = editorIDs;
		}

		void PaletteUnlockData::Populate(ResourceKey species = {}) {
			PopulateFromCreation(species);
			PopulateFromCollection();
			UpdateCollection();
		}

		///------------------------------------------
		/// Interface with Unlock Config
		
		// Store and return a list of palette category IDs from the mPartUnlockConfigID
		vector<uint32_t> GetUnlockCategories() {
			if (mPartUnlockCategories.size() > 0) return mPartUnlockCategories;

			if (mPartUnlockConfigID != 0x0) {
				// partUnlocks~
				uint32_t partUnlockFeedList = CapabilityChecker::GetModelUInt32Value({ mPartUnlockConfigID, prop, 0x7b2c1325 }, 0x055DD9F6);
				if (partUnlockFeedList != 0x0) {
					// AssetBrowserFeedList
					vector<ResourceKey> feedListCategories = CapabilityChecker::GetModelKeyValues({ partUnlockFeedList, prop, 0x6edc12d4 }, 0x744717C0);
					for (size_t i = 0; i < feedListCategories.size(); i++) {
						// AssetBrowserFeedCategories
						vector<ResourceKey> feedListItems = CapabilityChecker::GetModelKeyValues({ feedListCategories[i].instanceID, prop, 0x06b38241 }, 0x744717C0);
						for (size_t j = 0; j < feedListCategories.size(); j++) {
							// AssetBrowserFeedItems
							uint32_t feedListItemUniqueID = CapabilityChecker::GetModelUInt32Value({ feedListCategories[i].instanceID, prop, 0x4e5892eb}, 0x06678DF3);
							mPartUnlockCategories.push_back(feedListItemUniqueID);
						}
					}
				}
			}
			return mPartUnlockCategories;
		}

		///------------------------------------------
		/// Interface with Items

		// Return an UnlockableItem* struct pointer from mUnlockedItems
		UnlockableItem* GetUnlockedItem(ResourceKey key) {
			for (size_t i = 0; i < mUnlockedItems.size(); i++) {
				if (mUnlockedItems[i].mKey.instanceID == key.instanceID && mUnlockedItems[i].mKey.groupID == key.groupID) {
					return &mUnlockedItems[i];
				}
			}
			return nullptr;
		}

		// Check if the list contains an UnlockableItem with this key
		bool IsItemUnlocked(ResourceKey key) {
			return bool(GetUnlockedItem(key));
		}

		// Turn a resource key into an UnlockableItem and add it to the list
		// TODO: should this return the created UnlockableItem?
		void PaletteUnlockData::Unlock(ResourceKey key) {
			if (!IsItemUnlocked(key)) {
				mUnlockedItems.push_back(UnlockableItem(key));
			}
			// update collection with new item
			if (GetCollection()) {
				GetCollection()->mUnlockedItems.push_back({ key.instanceID, key.groupID });
			}
		}

		// Set mbNewItem on every item to false
		void PaletteUnlockData::MarkItemsRead() {
			for (size_t i = 0; i < mUnlockedItems.size(); i++) {
				mUnlockedItems[i].mbNewItem = false;
			}
		}

		///------------------------------------------
		/// Interface with Collections

		cCollectableItemsPtr GetCollection() {
			switch (mCollectionID) {
			case CLG: return StarManager.mpGlobalCLGItems; // Cell Parts
			case CRG: return GetPlayer()->mpCRGItems; // Creature Parts
			}
			return nullptr;
		}

		// Adds items from a collection into the unlock data
		void PaletteUnlockData::PopulateFromCreation(ResourceKey& creation, bool associated_parts = true)
		{
			auto unlocks = AssociatedPartUnlocks(creation, associated_parts);
			for (size_t i = 0; i < unlocks.mUnlockedParts.size(); i++) {
				Unlock(unlocks.mUnlockedParts[i]);
			}
		}

		// Adds items from the associated collection into the unlock data
		void PaletteUnlockData::PopulateFromCollection()
		{
			auto collection = GetCollection();
			if (!collection) return;
			for (auto item : collection->mUnlockedItems) {
				Unlock(ResourceKey(item.instanceID, prop, item.groupID));
			}
		}

		// Adds items from mUnlockedItems into Collections
		void PaletteUnlockData::UpdateCollection()
		{
			auto collection = GetCollection();
			if (!collection) return;
			collection->mUnlockedItems.clear();
			for (size_t i = 0; i < mUnlockedItems.size(); i++) {
				collection->mUnlockedItems.push_back({ mUnlockedItems[i].mKey.instanceID, mUnlockedItems[i].mKey.groupID });
			}
		}

		static Simulator::Attribute ATTRIBUTES[];
	};

	// Data struct that creates a list of parts to be unlocked on startup from that creature
	// Gethered from a creature's rigblock list.
	struct AssociatedPartUnlocks {

		vector<ResourceKey> mUnlockedParts;

		AssociatedPartUnlocks(ResourceKey& creation, bool associated_parts = true) {

			// Loop through all parts on the creature.
			vector<ResourceKey> parts = CapabilityChecker::GetCreationRigblocks(creation);
			bool loaded_vertebra = false;
			for (size_t n = 0; n < parts.size(); n++) {

				// Exclude symmetric variants
				if (CapabilityChecker::IsModelSymmetricVariant(parts[n])) {
					continue;
				}
				// Only load vertebra once.
				bool is_spine = CapabilityChecker::GetModelIntValue(parts[n], 0x100F0F5A) > 0;
				bool is_limb = CapabilityChecker::GetModelIntValue(parts[n], 0xB00F0FDF) > 0 || CapabilityChecker::GetModelIntValue(parts[n], 0x04DBCDD2) > 0;
				if (is_spine) {
					if (!loaded_vertebra) loaded_vertebra = true;
					else continue;
				}
				// TODO: check if the parts exist in the current palette and add the actual part to the list.


				ResourceKey parent = CapabilityChecker::GetModelParentKey(parts[n]);
				if (parent.groupID == 0x40626000 || parent.groupID == 0x40616000) continue; // Symmetric Parts
				
				// Also, since these KVs are only in the parent template file, make sure that they are properly loaded in child parts we are actually checking.
				// TODO: test without using parent. may have fixed that bug?
				ResourceKey& part = is_spine ? parent : parts[n];

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

				// Unlock parts from the above data. Don't worry about duplicates yet.
				vector<ResourceKey> unlocked;

				if (!is_spine && !is_limb) {
					// Always unlock the part itself
					unlocked.push_back(parts[n]);
				}

				for (size_t i = 0; i < (size_t)5; i++) {
					// Unlock all from list 0
					if (i == 0) {
						for (size_t j = 0; j < unlocks[i].size(); j++) {
							unlocked.push_back(unlocks[i][j]);
						}
					}
					// Unlock randomly from lists 1-5
					// TODO: implement checks to make sure that each part being unlocked is not already in mUnlockedParts / unlocked?
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
								
								unlocked.push_back(unlocks[i][rand_idx]);
							}
						}
					}
				}
				// Remove duplicates from unlocked list and add to mUnlockedParts
				for (size_t i = 0; i < unlocked.size(); i++) {
					bool found = false;
					for (size_t j = 0; j < mUnlockedParts.size(); j++) {
						if (mUnlockedParts[j] == unlocked[i]) {
							found = true;
							break;
						}
					}
					if (found) { continue; }
					else { mUnlockedParts.push_back(unlocked[i]); }
				}
			}
		}
	
	};

	//------------------------------------------------------------------------------------------

public:
	static const uint32_t TYPE = id("Valla_HorizonBlue::cPaletteUnlock");
	static const uint32_t NOUN_ID = TYPE;
	static const uint32_t kImgPanelID = 0x4DEDDD52;
	vector<uint32_t> messages = {
		// Spore
		kMsgSwitchGameMode,
		uint32_t(kMessageLoadGame),
		// Palette Unlocks
		id("UnlockPart"),
	};

	cPaletteUnlock();
	~cPaletteUnlock();

	int AddRef() override { return Simulator::cStrategy::AddRef(); }
	int Release() override { return Simulator::cStrategy::Release(); }
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override { return "Valla_HorizonBlue::cPaletteUnlock"; }
	bool Write(Simulator::ISerializerStream* stream) override;
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	static cPaletteUnlock* Get() { return sInstance; }
	//------------------------------------------

	// Map stage ID to PaletteUnlockData instanced with affected palette IDs
	// Because of the way the palette unlocks work, you can use the same palette unlock data for multiple editors.
	hash_map<uint32_t, PaletteUnlockData> mPaletteUnlocks;
		// EG: { kGameCell, PaletteUnlockData({0x00000001, 0x00000005})},

	// Store current ingame palette
	PaletteUI* mpLastPalette = nullptr;
	void EditorPaletteUILoad(PaletteUI* palette); // auto-called
	void EditorPaletteUILoadDone(PaletteUI* palette); // auto-called

	void ClearSaveData(); // create the mPaletteUnlocks structure when entering a save
	void InitializeSaveData(); // create the mPaletteUnlocks structure when entering a save
	bool IsValidPlayer() const; // check if mPlayerID matches the save ID

	// Synchronizes CRG collections with unlocked items,
	// One to pull in any unlocked parts from collections into the data,
	// and one to set collections to match the unlocked parts in the data
	void PopulateUnlockDataFromCollection(PaletteUnlockData* data, cCollectableItemsPtr collection);
	void UpdateCollectionFromUnlockData(cCollectableItemsPtr collection, PaletteUnlockData* data);
	// Populate the unlock data from a creation's rigblocks and associated part unlocks
	void PopulateUnlockDataFromCreation(PaletteUnlockData* data, ResourceKey& creation, bool associated_parts = true);
	// TODO: Populate the unlock data with any part that has an itemUnlockFindPercentage >= 1.0

	// Get palette from current editor or last palette
	PaletteUIPtr GetCurrentPaletteUI() const;
	// Get all palette itemUIs. Optionally limit to these category IDs
	vector<StandardItemUIPtr> GetPaletteItemUIs(vector<uint32_t> categories = {}) const;
	// TODO: get a palette item by its position in the page V2(across,down)
	StandardItemUIPtr GetPaletteItemByPageCoords(PalettePageUIPtr page, Vector2 pos) const;


	// Apply a PaletteUnlockData* to the active editor (or pLastPalette)
	void EditorApplyPaletteUnlockData(PaletteUnlockData* data);

	//------------------------------------------
	
	bool HandleMessage(uint32_t messageID, void* msg) override;

	//------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];


protected:
	static cPaletteUnlock* sInstance;


private:
	const ColorRGBA mColorPaletteItemDisabled = ColorRGBA(0.04f, 0.048f, 0.066f, 0.65f);
	const uint32_t kGraphicsAtlas = 0x31a44893;

	// Palette Item Actions
	bool IsItemHidden(StandardItemUIPtr item) const;
	void ItemHide(StandardItemUIPtr item);
	void ItemShow(StandardItemUIPtr item);
	IWindow* ItemCreateImgPanel(StandardItemUIPtr item, ResourceKey& icon, IWindow* parent = nullptr);
	void ItemRemoveImgPanel(StandardItemUIPtr item);
	//
	void ItemSetDisabled(StandardItemUIPtr item);
	void ItemSetLocked(StandardItemUIPtr item);
	void ItemSetNewHighlight(StandardItemUIPtr item);
	void ItemReset(StandardItemUIPtr item);


	// Part rigblock info
	// Get only rigblocks that appear in this data's editor palettes
	vector<ResourceKey> GetCreatureUnlockableRigblocks(ResourceKey& creature, PaletteUnlockData* data) const;
	//vector<ResourceKey> GetAssociatedPartUnlocks(ResourceKey& key) const;

	// ID to track which savegame data this belongs to. If it doesn't match the current game, reset all data.
	uint32_t mSaveGameID = 0x0;

};

