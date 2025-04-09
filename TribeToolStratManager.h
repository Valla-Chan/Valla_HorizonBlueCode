#pragma once

#include <Spore\BasicIncludes.h>
#include "cTribeToolStrategy.h"
#include "TribeToolDataKeys.h"

#define cTribeToolStratManagerPtr intrusive_ptr<cTribeToolStratManager>
#define TribeToolStratManager (*cTribeToolStratManager::Get())


class cTribeToolStratManager 
	: public Object
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("TribeToolStratManager");
	
	cTribeToolStratManager();
	~cTribeToolStratManager();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	static cTribeToolStratManager* Get();

	//------------------------------------------------------------
	// Tool Data

	// Additional data for new tribe tools
	struct ToolMetadata {
		int mHandHeldIndex = 0;
		uint32_t mHandHeldItemEffect = 0x0;
		uint32_t mToolEnRouteAnim = 0x0;
		//uint32_t mToolGrabAnimOverride = 0x0; // todo, figure out how i want to use this

		bool mbToolPrebuilt = false;
		bool mbToolAllowMultiple = false;
		bool mbToolDisableRollover = false;

		// turn this on depending on various factors, like the if the grab anim ID is overridden or the tool shed has an ability attached, or just a simple boolprop to force-enable it
		//bool mbToolAllowUse = false;

		ResourceKey mSpecializedName = {};
		ResourceKey mAbilityKey = {};
	};

	hash_map<int, uint32_t> tribeToolIDs;
	hash_map<int, ToolMetadata*> tribeToolMetadata;

	//------------------------------------------------------------
	// Strategies
	//------------------------------------------------------------
	static void AddStrategy(cTribeToolStrategy* pStrategy, uint32_t id);
	static cTribeToolStrategyPtr GetStrategy(uint32_t id);

	//------------------------------------------------------------
	// Helper funcs
	//------------------------------------------------------------
	//
	// Tools
	//---------
	static cTribeToolPtr GetHoveredTool(bool playerOwned = true);
	/// TODO: returns true if the tool has an ability/item associated with it, or otherwise can be interacted with.
	static bool CanToolBeUsed(cTribeToolPtr tool);
	
	//// Tribe Members
	//-----------------
	static vector<cCreatureCitizenPtr> GetSelectedMembers();
	static void UseTool(vector<cCreatureCitizenPtr> citizens, cTribeToolPtr tool);
	static void UseTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool);
	/// Tell the selected citizen creatures to use a tool.
	static void SelectedUseTool(cTribeToolPtr tool);


private:

protected:
	static cTribeToolStratManager* sInstance;
	static eastl::hash_map<uint32_t, cTribeToolStrategyPtr> mStrategies;
};
