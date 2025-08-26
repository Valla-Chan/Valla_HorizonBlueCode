#pragma once

#include <Spore\BasicIncludes.h>
#include "CapabilityChecker.h"

#define CRG_InventoryPtr intrusive_ptr<CRG_Inventory>

using namespace UTFWin;
using namespace Simulator;
using namespace TypeIDs;

class CRG_Inventory
	: public Simulator::cStrategy
	, public IWinProc
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("Valla_CreatureOverhaul::CRG_Inventory");
	static const uint32_t NOUN_ID = TYPE;
	const uint32_t itemPath = id("CRG_Inventory");

	enum EventUIwindows {
		// 3888770464 - 3888770470
		Slot0 = 0xE7C9EDA0,
		Slot1 = 0xE7C9EDA1,
		Slot2 = 0xE7C9EDA2,
		Slot3 = 0xE7C9EDA3,
		Slot4 = 0xE7C9EDA4,
		Slot5 = 0xE7C9EDA5,
		Slot6 = 0xE7C9EDA6,
	};
	enum FoodType {
		Fruit = id("food_fruit"),
		Meat = id("food_meat"),
	};
	enum FoodStoreAnims {
		StoreFruit = id("crg_storefruit"),
		StoreMeat = id("crg_storemeat"),
	};

	// Eating animations to detect
	const vector<uint32_t> eatFruitAnims = {
		// Hand
		0x04EEA0D5,
		0x04EEA0D6,
		0x04F7D4DF,
		0x04F7D54A,
		// Mouth
		0x04EEA0D2,
		0x04EEA0D3,
		0x04F7D586,
		0x04F7D5BC,
	};
	const vector<uint32_t> eatMeatAnims = {
		// Hand
		0x04F65994,
		0x04F7C945,
		// Mouth
		0x04F65995,
		0x04F7C95E,
	};

	//-------------------------------

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t typeID) const override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;

	//-------------------------------------------------------

	/// Return true if the food eaten should be stored
	bool ShouldStoreFood() const;
	uint32_t GetFoodStoreAnim(uint32_t animID);
	void StoreFood(uint32_t foodType);

	bool AddItemToInventory(uint32_t itemID);
	void ConsumeItemAtSlot(int index);
	void RemoveItemAtSlot(int index);

	ResourceKey GetItemResource(uint32_t itemID) const;
	ResourceKey GetItemImage(ResourceKey itemKey) const;
	int GetOpenSlotIndex() const;
	IWindow* GetPopupWindow() const;
	IWindow* GetSlotWindow(int index) const;
	ResourceKey GetSlotItemResource(int index) const;
	void SetSlotItem(uint32_t itemID, int index);

	void DelayCreatureAgeCheck();
	void CheckCreatureAge();

	void ClickInventoryItem(int index);
	void EnableInventory();
	void DisableInventory();
	void ClearInventory();

	//-------------------------------------------------------
	int GetEventFlags() const override;
	/// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;

	static Simulator::Attribute ATTRIBUTES[];

private:

	fixed_vector<uint32_t, 7> mInventory = {}; // stores item IDs from CRG_Inventory folder
};