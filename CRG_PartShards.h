#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\UTFWin\InflateEffect.h>
#include <Spore\UTFWin\GlideEffect.h>
#include "CapabilityChecker.h"

#define CRG_PartShardsPtr intrusive_ptr<CRG_PartShards>


using namespace UTFWin;
using namespace Simulator;
using namespace UI;

class CRG_PartShards
	: public IWinProc
	, public DefaultRefCounted
	, public Simulator::cStrategy
{
public:
	static const uint32_t TYPE = id("CRG_PartShards");
	static const uint32_t NOUN_ID = TYPE;

	CRG_PartShards();
	~CRG_PartShards();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t typeID) const override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	bool Read(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	void Update(int deltaTime, int deltaGameTime) override;

	//------------------------------------------------------

	// Shards UI
	void AddShards(int shardcount);
	void SubtractShards(int shardcount);
	void SetShardCount(int shardcount);
	int GetShardCount() const;
	bool HasEnoughShards() const;

	// Part Redeem / Destroy
	bool IsPartDestroyed(ResourceKey partkey) const;
	bool IsPartUsedOnCreature(ResourceKey partkey, cCreatureBase* creature) const;
	bool IsPartMouth(ResourceKey partkey) const;
	bool IsPartWing(ResourceKey partkey) const;
	bool IsPartDetail(ResourceKey partkey) const;
	bool IsPartHandFootWeapon(ResourceKey partkey) const;
	bool IsPartLimb(ResourceKey partkey) const;
	bool IsPartCellPart(ResourceKey partkey) const;
	int GetPartUnlockLevel(ResourceKey partkey) const;

	// Collections UI
	void AddPartWindow(IWindow* pWindow, const ResourceKey& imageName);
	void TryCollectionsTabOpened();
	void ShowNewPartUnlock();

	//------------------------------------------------------
	int GetEventFlags() const override;
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

	static Simulator::Attribute ATTRIBUTES[];

private:
	// Gamble Button Strings
	const LocalizedString localeShardsExtractTxt = { id("Tooltips_HB") , 0x000CAFC4 };
	const LocalizedString localeShardsExtractDesc = { id("Tooltips_HB") , 0x000CAFB4 };
	const LocalizedString localeShardsExtractDescInvalid = { id("Tooltips_HB") , 0x000CAFB8 };

	const LocalizedString localeShardsRedeemTxt = { id("Tooltips_HB") , 0x000CAFC5 };
	const LocalizedString localeShardsRedeemDesc = { id("Tooltips_HB") , 0x000CAFB5 };

	const LocalizedString localeShardsUnlockTxt = { id("Tooltips_HB") , 0x000CAFC6 };
	const LocalizedString localeShardsUnlockTxt2 = { id("Tooltips_HB") , 0x000CAFC7 };
	const LocalizedString localeShardsUnlockDesc = { id("Tooltips_HB") , 0x000CAFB6 };
	const LocalizedString localeShardsUnlockDescInvalid = { id("Tooltips_HB") , 0x000CAFB7 };

	const LocalizedString localePartInUse = { id("Tooltips_HB") , 0x000CAFCC };

	// Part Value Strings
	const LocalizedString localePartValueTxt = { id("Tooltips_HB") , 0x000CAFC3 };
	const LocalizedString localePartCostTxt = { id("Tooltips_HB") , 0x000CAFD3 };
	const LocalizedString localePartRedeemTxt = { id("Tooltips_HB") , 0x000CAFD4 };

	const char16_t* char_shard = u"▲";

	enum GambleState {
		Extract,
		Redeem,
		Unlock,
	};

	enum SelectionType {
		Sell,
		Buy,
	};


	int mPrevShards = 0;
	int mShards = 0;
	int mbCollectionsLoadedOnce = false;
	bool mbSupressCollectionsOpened = false;

	// part shard values
	const int mMinShardsToGamble = 15;

	const int mPartShardValueCell = 3; // how much this part scraps for (cell parts)
	const int mPartShardValue = 5; // how much this part scraps for
	const int mPartShardValuePerTier = 2; // add this much to the value per tier

	const int mPartShardCostCell = 7; // how much to buy back for (cell parts)
	const int mPartShardCost = 10; // how much to buy back for
	const int mPartShardCostPerTier = 5; // add this much to the cost per tier


	const ColorRGBA mColorPartDisabled = ColorRGBA(0.22f, 0.22f, 0.22f, 0.28f);
	const ColorRGBA mColorPartInUse = ColorRGBA(0.9f, 0.9f, 0.9f, 0.70f);
	const ColorRGBA mColorPartPaletteDisabled = ColorRGBA(0.04f, 0.048f, 0.066f, 0.65f);

	ScheduledTaskListenerPtr schedule_closepartunlock;

	//---------------------------------------------
	// Shards UI
	void UpdateShardCount();
	void UpdatePartValue();
	void UpdateGambleButtonState();
	void SetGambleButtonState(int state);
	void SetGambleButtonEnabled(bool state);
	void GambleButtonPressed();

	//---------------------------------------------
	// Part Redeem / Destroy
	void RedeemPart(int tier, int price);
	void ClosePartUnlockWindow();
	void SetPartUnlockWindowState(bool state, ResourceKey part = {});

	void DestroySelectedParts(); // destroy and cash out all selected parts
	void ClearSelectedParts(); // visually clear part selection
	void ClearSelectionSimple(); // clear part selection interally
	void UnlockSelectedParts(); // re-unlock the selected locked parts.

	//---------------------------------------------
	// Collections UI
	int mSelectionType = Sell;
	vector<ResourceKey> mSelectedParts;
	vector<IWindow*> mSelectedPartWindows;
	vector<ResourceKey> mDestroyedParts; // to suppress in the editor. this will need to be saved and the parts re-deleted on game load.

	bool mbAllMouthsSelected = false;
	int mMouthsSelected = 0;
	int mMouthsDestroyed = 0;
	vector<ResourceKey> mMouthParts;
	

	void CollectionsTabOpened(); // changed tabs
	void ResetCollectionsOpenedBool();

	bool IsCollectionsOpen() const;
	bool ArePartsSelected() const;
	void SetPartSelectionType(int selectiontype);

	int GetPartShardValue(ResourceKey partkey) const; // shard value of a single part
	int GetPartShardBuybackCost(ResourceKey partkey) const; // shard value of a single part
	int GetAllPartsShardValue() const; // shard value of all parts
	int GetAllPartsShardBuybackCost() const;

	ResourceKey GetPartKeyFromWindow(IWindow* window) const;
	ResourceKey GetPartKeyFromID(uint32_t imageID) const;

	// Mouths

	bool AreAllMouthsSelected() const;

	//------------------
	// Part Windows

	bool IsPartWindowValid(IWindow* window); // return true if this window has a part
	bool IsPartWindowSelected(IWindow* window);
	void SetPartWindowSelected(IWindow* window, bool selected); // select or deselect a part window, and store or erase its data.

	bool IsPartWindowUnlocked(IWindow* window); // return true if this window has a part and is unlocked
	bool IsPartWindowInUse(IWindow* window); // return true if this window has a part and is unlocked
public: void SetPartWindowUnlocked(IWindow* window, bool state);
	void PartWindowHovered(IWindow* window);
private:
	

	//---------------------------------------------
	// Editor

	bool IsPaletteItemDestroyed(StandardItemUIPtr item) const;
	void ProcessPaletteItem(StandardItemUIPtr item) const;
public:void SuppressDestroyedParts();
private:

	Tooltip* GetWindowTooltip(IWindow* window) const;
	IGlideEffect* CRG_PartShards::GetWindowGlide(IWindow* window) const;
	IInflateEffect* GetWindowInflate(IWindow* window) const;
};