#pragma once

#include <Spore\BasicIncludes.h>
#include "TRG_ToolIDs.h"
#include "TribeToolDataKeys.h"

#define cTribeToolManagerPtr intrusive_ptr<cTribeToolManager>
#define TribeToolManager (*cTribeToolManager::Get())

using namespace UTFWin;
using namespace Simulator;
using namespace TypeIDs;

const Vector2 mTribeToolScaleBounds = Vector2(0.75f, 1.25f);
const Vector2 mTribeHutScaleBounds = Vector2(2.5f, 3.5f);

class cTribeToolManager 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("TribeToolManager");
	
	cTribeToolManager();
	~cTribeToolManager();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;
	static cTribeToolManager* Get();

//------------------------------------------------------------
// Tool Data Format
//------------------------------------------------------------


	// Additional data for new tribe tools
	struct ToolMetadata {
		int mHandHeldIndex = 0;
		uint32_t mHandHeldItemEffect = 0x0;
		uint32_t mToolEnRouteAnim = 0x0;
		uint32_t mToolGetToolAnim = 0x0; // todo, figure out how i want to use this. maybe if 0x0, do not override.
		
		uint32_t mToolStrategyID = 0x0;

		Vector2 mToolScaleBounds = mTribeToolScaleBounds;

		bool mbToolPrebuilt = false;
		bool mbToolAllowMultiple = false;
		bool mbToolDisableRollover = false;

		// turn this on depending on various factors, like the if the grab anim ID is overridden or the tool shed has an ability attached, or just a simple boolprop to force-enable it
		//bool mbToolAllowUse = false;

		ResourceKey mSpecializedName = {};
		ResourceKey mAbilityKey = {};
	};


	hash_map<int, uint32_t> mTribeToolIDs; // map toolType ID to resource instance ID
	hash_map<int, ToolMetadata*> mTribeToolMetadata; // map toolType ID to tool metadata pointer

	//hash_map<int, cTribeToolData*> mTribeToolData; // hb tribal tool data.
	
	// TODO: seemingly not in use? may be a good idea to cache this if possible, like the metadata, and only override that if needed. see above.
	//eastl::array<cTribeToolData*, 64> mToolDataArray; // hb tribal tool data.


//----------------------------------------------------------------------------------------------------------------------------------
// Tooldata Parsing
//----------------------------------------------------------------------------------------------------------------------------------

	void PopulateTribeToolData(); // pull the data from the files into memory
	ToolMetadata* GetTribeToolMetadata(int toolType) const;
	cTribeToolData* GetTribeToolData(int toolType) const;
	static cTribeToolData* TribeToolDataFromProp(ResourceKey key, int typeIDoverride = -1);
	static int TribeToolTypeIDFromProp(ResourceKey key);
	static ToolMetadata* TribeToolMetadataFromProp(ResourceKey key);
	static int GetNewHomeID(cTribePtr tribe);

//----------------------------------------------------------------------------------------------------------------------------------
// Helper Functions
//----------------------------------------------------------------------------------------------------------------------------------

// Tribe Plan
//------------
	static bool TribeHasToolType(cTribePtr tribe, int tooltype);
	// TODO: this should be eventually moved to TribePlanManager, once that is made into a singleton.
	static bool IsPlannerOpen();
	bool HoveringScalableObject();

// Tools
//---------
	static cTribeToolPtr GetHoveredTool(bool playerOwned = true);
	void ScaleTribeTool(cTribeToolPtr tool, float amount);
	void ScaleTribeHut(cTribeHutPtr hut, float amount);
	void PlayBuildingScaleSound();
	/// TODO: returns true if the tool has an ability/item associated with it, or otherwise can be interacted with.
	static bool CanToolBeUsed(cTribeToolPtr tool);
	static bool IsToolFirePit(cTribeToolPtr tool);
	static bool IsToolHome(cTribeToolPtr tool);
	static bool IsToolPlayerOwned(cTribeToolPtr tool);
	static bool IsHutPlayerOwned(cTribeHutPtr hut);

// Tribe Members
//-----------------

	// TODO: cTribeInputStrategy might have better copies of these that already exist, but I cannot seemingly call them externally. investigate.
	// DoActionGeneric, ChooseInputActionID, SetHoverObjectCursorAndRollover, etc may need to be detoured.
	//
	// consider making the actions on L/R click be data driven? so you can right click to send a creature over to use an item, but you can also send them over to destroy it even if they own the tool.
	// and handle what happens when clicking on it with low health, keeping in mind that the repair action is default.

	static vector<cCreatureCitizenPtr> GetSelectedMembers();
	static void UseTool(vector<cCreatureCitizenPtr> citizens, cTribeToolPtr tool);
	static void UseTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool);
	/// Tell the selected citizen creatures to use a tool.
	static void SelectedUseTool(cTribeToolPtr tool);

// Contruction Progress
//----------------------

	bool mbHutsPrebuilt = false; // true = default behavior
	vector<cTribeToolPtr> mpInProgressTools = {};
	vector<float> mpPlotHealths = {};

	void UpdateToolsProgress(); // check if under construction tool has been completed
	void RemoveFromProgress(cTribeToolPtr tool);
	ResourceKey GetPlotModel(cTribeToolPtr tool) const;
	cTribeToolPtr GetHoveredConstructionPlot() const;
	bool IsConstructionPlotHovered() const;
	bool IsToolInProgress(cTribeToolPtr tool) const;

	void AddedTool(cTribeToolPtr tool, cTribePtr tribe); // new tool has just been added
	void ToolComplete(cTribeToolPtr tool); // tool has finished construction

// Handheld Items
//----------------
	
	vector<Vector2> mToolQueueCitizenData; // ToolID, HandheldID

	// Add a chief creature's diet value to the queue
	void AddCitizenToolTypeToQueue(int toolID, int handheldID);
	void RemoveCitizenToolTypeQueue();
	uint32_t ConvertToolEffectID(uint32_t instanceId);
	//uint32_t GetGrabOverrideAnim() const; // TODO, will probably require metadata.
	ResourceKey GetCitizenNameLocaleResource(cCreatureCitizenPtr citizen) const;

// Messages
//----------------

	int GetEventFlags() const override;
	virtual bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
protected:
	static cTribeToolManager* sInstance;

private:
	bool mbShiftHeld = false;
};


