#pragma once

#include "cPaletteUnlockStructs.h"

#define cPaletteUnlockPtr intrusive_ptr<cPaletteUnlock>
#define PaletteUnlockManager (*cPaletteUnlock::Get())

using namespace UTFWin;
using namespace UI;
using namespace Simulator;
using namespace Editors;
using namespace Palettes;
using namespace TypeIDs;

class cPaletteUnlock
	: public cPaletteUnlockStructs
	, public Simulator::cStrategy
	, public App::IMessageListener
{

public:
	//------------------------------------------------------------------------------------------

public:
	static const uint32_t TYPE = id("Valla_HorizonBlue::cPaletteUnlock");
	static const uint32_t NOUN_ID = TYPE;
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
	void EditorPaletteUILoad(PaletteUI* palette); // auto-called when the editor begins loading a palette
	void EditorPaletteUILoadDone(const PaletteUI* palette); // auto-called when the editor loads a creature model

	void ClearSaveData(); // create the mPaletteUnlocks structure when entering a save
	void InitializeSaveData(); // create the mPaletteUnlocks structure when entering a save
	bool IsValidPlayer() const; // check if mPlayerID matches the save ID


	// Get palette from current editor or last palette
	PaletteUIPtr GetCurrentPaletteUI() const;

	// Apply a PaletteUnlockData* to the active editor (or pLastPalette)
	void EditorApplyPaletteUnlockData(PaletteUnlockData* data);

	//------------------------------------------
	
	bool HandleMessage(uint32_t messageID, void* msg) override;

	//------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];


protected:
	static cPaletteUnlock* sInstance;


private:

	// Part rigblock info
	// Get only rigblocks that appear in this data's editor palettes
	vector<ResourceKey> GetCreatureUnlockableRigblocks(ResourceKey& creature, PaletteUnlockData* data) const;
	//vector<ResourceKey> GetAssociatedPartUnlocks(ResourceKey& key) const;

	// ID to track which savegame data this belongs to. If it doesn't match the current game, reset all data.
	uint32_t mSaveGameID = 0x0;


// Detour helper stuff
public:
	bool bLoadPartCoords = false;
	hash_map<ResourceKey, Vector2> pLoadedPartCoords;
	void ED_ReadItemsModule_detour(const ResourceKey& moduleName);
};

