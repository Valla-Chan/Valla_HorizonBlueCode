// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore/Effects.h>
#include <Spore/Swarm/cEffectsManager.h>
#include <Spore\Simulator\cCreatureGameData.h>

// UI
#include "UI_Timescale.h"
#include "UI_RenamePlanet.h"

// Misc
#include "Cheats/HBCheats.h"
#include "HBAssetBrowser.h"
#include "HBDebugFuncs.h"

// Stages
#include "HBCell.h"
#include "HBCrg.h"
#include "HBTribe.h"
#include "HBCiv.h"
#include "HBSpace.h"
#include "HBEp1.h"

// Singletons
#include "CreatureSpeedBoost.h"

// Scripts
#include "CapabilityChecker.h"

// Game Modes
#include "ArenaMode.h"


void Initialize()
{
	// Arena Mode
	//auto arena = new ArenaMode();
	//GameModeManager.AddGameMode(arena, ArenaMode::MODE_ID, "ArenaMode");

	// Singletons
	auto creaturespeedboost = new(CreatureSpeedBoost);

	// UI / General Simulator
	UI_Timescale* uitimescale = new(UI_Timescale);
	UI_RenamePlanet* uirenameplanet = new(UI_RenamePlanet);

	// Misc
	HBCheats::Initialize();
	HBAssetBrowser::Initialize();

	// Stages
	HBCell::Initialize();
	HBCrg::Initialize();
	HBTribe::Initialize();
	HBCiv::Initialize();
	HBSpace::Initialize();
	HBEp1::Initialize();
}

// This method is called when the game is closing
void Dispose()
{
	// Misc
	HBCheats::Dispose();
	HBAssetBrowser::Dispose();

	// Stages
	HBCell::Dispose();
	HBCrg::Dispose();
	HBTribe::Dispose();
	HBCiv::Dispose();
	HBSpace::Dispose();
	HBEp1::Dispose();
	
}


// Detour the playanimation ID-picking func
member_detour(AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

		while (true) {
			if (CRG_AnimOverride_detour(this, animID, pChoice)) break;
			if (TRG_AnimOverride_detour(this, animID, pChoice)) break;
			if (CVG_Convo_AnimOverride_detour(this, animID, pChoice)) break;
			if (EP1_AnimOverride_detour(this, animID, pChoice)) break;
			if (AssetBrowser_AnimOverride_detour(this, animID, pChoice)) break;
			if (Debug_AnimOverride_detour(this, animID, pChoice)) break;
			break;
		}
		return original_function(this, animID, pChoice);
	}
};

// Spui loading/spawning detour
member_detour(ReadSPUI_detour, UTFWin::UILayout, bool(const ResourceKey&, bool, uint32_t)) {
	bool detoured(const ResourceKey& name, bool arg_4, uint32_t arg_8) {
		ResourceKey& res = ResourceKey(name);

		while (true) {
			if (CRG_ReadSPUI_detour(this, res, arg_4, arg_8)) break;
			if (TRG_ReadSPUI_detour(this, res, arg_4, arg_8)) break;
			//if (CVG_ReadSPUI_detour(this, res, arg_4, arg_8)) break;
			//if (SPG_ReadSPUI_detour(this, res, arg_4, arg_8)) break;
			break;
		}
		return original_function(this, res, arg_4, arg_8);
	}
};

//detour settext in LocalizedString
member_detour(LocalStringSetText_detour, LocalizedString, bool(uint32_t, uint32_t, const char16_t*)) {
	bool detoured(uint32_t tableID, uint32_t instanceID, const char16_t* pPlaceholderText) {
		while (true) {
			if (TRG_LocalStringSetText_detour(this, tableID, instanceID)) break;
			if (CVG_LocalStringSetText_detour(this, tableID, instanceID)) break;
			break;
		}
		return original_function(this, tableID, instanceID, pPlaceholderText);
	}
};


// Detour the model setting func
virtual_detour(SetModel_detour, cSpatialObject, cSpatialObject, void(const ResourceKey&)) {
	void detoured(const ResourceKey& modelKey) {
		ResourceKey& res = ResourceKey(modelKey);
		while (true) {
			if (CRG_SetModel_detour(this, res)) break;
			if (TRG_SetModel_detour(this, res)) break;
			break;
		}
		original_function(this, modelKey);
	}
};

// Detour the cursor setting func
member_detour(SetCursor_detour, UTFWin::cCursorManager, bool(uint32_t)) {
	bool detoured(uint32_t id) {
		while (true) {
			if (CRG_SetCursor_detour(this, id)) break;
			if (TRG_SetCursor_detour(this, id)) break;
			if (CVG_SetCursor_detour(this, id)) break;
			if (CreaturePickup_SetCursor_detour(this, id)) break;
			break;
		}
		return original_function(this, id);
	}
};

// Called when a combatant takes damage
virtual_detour(CombatTakeDamage_detour, Simulator::cCombatant, Simulator::cCombatant, int(float, uint32_t, int, const Vector3&, cCombatant*))
{
	int detoured(float damage, uint32_t attackerPoliticalID, int integer, const Vector3 & vector, cCombatant * pAttacker)
	{
		while (true) {
			if (CRG_CombatTakeDamage_detour(this, damage, attackerPoliticalID, integer, vector, pAttacker)) break;
			if (TRG_CombatTakeDamage_detour(this, damage, attackerPoliticalID, integer, vector, pAttacker)) break;
			break;
		}
		return original_function(this, damage, attackerPoliticalID, integer, vector, pAttacker);
	}
};

// Detour GetRolloverIdForObject in SimulatorRollover
static_detour(GetRolloverIdForObject_detour, UI::SimulatorRolloverID(cGameData*)) {
	UI::SimulatorRolloverID detoured(cGameData* object) {

		UI::SimulatorRolloverID value = original_function(object);
		while (true) {
			if (CRG_GetRolloverIdForObject_detour(object, value)) break;
			if (TRG_GetRolloverIdForObject_detour(object, value)) break;
			break;
		}
		return value;
	}
};


// Detour the effect playing func
member_detour(EffectOverride_detour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId) // Detour the function for obtaining effect indexes
	{
		// detect if a cinematic is beginning and emit a message
		if (instanceId == id("fade_to_black_1") || instanceId == id("fade_to_black_3") || instanceId == id("fade_to_black_quick")) {
			MessageManager.MessageSend(id("ResetTimescale"), nullptr);
		}

		// detect if a baby is growing up and send a message or suppress FX.
		if (instanceId == 0x3A616FEE) {
			MessageManager.MessageSend(id("BabyGrowUp"), nullptr);
		}
		while (true) {
			if (TRG_EffectOverride_detour(this, instanceId, groupId)) break;
			break;
		}
		Debug_EffectOverride_detour(instanceId);

		return original_function(this, instanceId, groupId); // Call the original function with the new instance ID.
	}
};

// Detour the UIEventLog ShowEvent func
member_detour(UIShowEvent_detour, cUIEventLog, uint32_t(uint32_t, uint32_t, int, Math::Vector3*, bool, int))
{
	uint32_t detoured(uint32_t instanceID, uint32_t groupID, int int1, Math::Vector3 * vector3, bool dontAllowDuplicates, int int2)
	{
		while (true) {
			if (TRG_UIShowEvent_detour(this, instanceID, groupID)) { return 0x0; }
			break;
		}
		CVG_UIShowEvent_detour(this, instanceID, groupID);
		return original_function(this, instanceID, groupID, int1, vector3, dontAllowDuplicates, int2);
	}
};

static_detour(ImageSetBackgroundByKey_detour, bool(IWindow*, const ResourceKey&, int imageIndex)) {
	bool detoured(IWindow * pWindow, const ResourceKey & imageName, int imageIndex) {
		auto value = original_function(pWindow, imageName, imageIndex);
		CRG_ImageSetBackgroundByKey_detour(pWindow, imageName, imageIndex);
		return value;
	}
};


// Detour the tribe spawning func
static_detour(TribeSpawn_detour, cTribe* (const Vector3&, int, int, int, bool, cSpeciesProfile*)) {
	cTribe* detoured(const Math::Vector3 & position, int tribeArchetype, int numMembers, int foodAmount, bool boolvalue, cSpeciesProfile * species) {
		while (true) {
			if (CVG_TribeSpawn_detour(position, tribeArchetype, numMembers, foodAmount, boolvalue, species)) break;
			break;
		}
		cTribe* tribe = original_function(position, tribeArchetype, numMembers, foodAmount, boolvalue, species);
		TribePlanManager.trg_hutmanager->SetupNewTribe(tribe);
		return tribe;
	}
};


// Editor parts palette loading func, PaletteUI::Load
member_detour(PaletteUILoad_detour, Palettes::PaletteUI, void(Palettes::PaletteMain*, UTFWin::IWindow*, bool, Palettes::PaletteInfo*)) {
	void detoured(Palettes::PaletteMain * pPalette, UTFWin::IWindow * pWindow, bool bool1, Palettes::PaletteInfo * pInfo) {
		original_function(this, pPalette, pWindow, bool1, pInfo);
		TRG_PaletteUILoad_detour(this);
	}
};

// PaletteUI::SetActiveCategory
member_detour(PaletteUISetActiveCategory_detour, Palettes::PaletteUI, void(int)) {
	void detoured(int categoryIndex) {

		// Wrap the member function `original_function` into a compatible function pointer
		auto func_ptr = [](Palettes::PaletteUI* palette, int index) {
			original_function(palette, index);
		};

		TRG_PaletteUISetActiveCategory_detour(categoryIndex, this, func_ptr);
		CVG_PaletteUISetActiveCategory_detour(categoryIndex);
		original_function(this, categoryIndex);
	}
};


//-----------------------------------

// TEMP?
member_detour(PaletteReadProp_detour, Palettes::PaletteUI, void(const ResourceKey&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)) {
	void detoured(const ResourceKey& name, uint32_t creationTypeID = -1, uint32_t arg_8 = 0,
				uint32_t layoutID = 0, uint32_t categoryLayoutID = 0, uint32_t pageLayoutID = 0, uint32_t arg_18 = 0) {

		//name = group 0x406b6a00 instance 0x0000000d type 0x00b1b104
		// id = 2661483290
		// arg8 = 0
		// layout 0
		// categoryLayoutID 0
		// pageLayoutID 0
		// arg_18 0
		original_function(this, name, creationTypeID, arg_8, layoutID, categoryLayoutID, pageLayoutID, arg_18);

		//if (name.instanceID == 0x0000000C) {
		//	original_function(this, ResourceKey(0x000000C9, 0x00b1b104, 0x406b6a00), creationTypeID, arg_8, layoutID, categoryLayoutID, pageLayoutID, arg_18);
		//}
	}
};


// TODO: the multiple detours are not working!
// They must be merged back into one func, with new sub-functions being called for each stage.
void AttachDetours()
{
	// Misc
	HBCheats::AttachDetours();
	HBAssetBrowser::AttachDetours();


	// Stages
	HBCell::AttachDetours();
	HBCrg::AttachDetours();
	HBTribe::AttachDetours();
	HBCiv::AttachDetours();
	HBSpace::AttachDetours();
	HBEp1::AttachDetours();
	
	// Standalone
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	EffectOverride_detour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));

	ReadSPUI_detour::attach(GetAddress(UTFWin::UILayout, Load));
	LocalStringSetText_detour::attach(GetAddress(LocalizedString, SetText));
	SetModel_detour::attach(GetAddress(Simulator::cSpatialObject, SetModelKey));
	SetCursor_detour::attach(GetAddress(UTFWin::cCursorManager, SetActiveCursor));

	ImageSetBackgroundByKey_detour::attach(GetAddress(UTFWin::Image, SetBackgroundByKey));
	CombatTakeDamage_detour::attach(GetAddress(Simulator::cCombatant, TakeDamage));
	GetRolloverIdForObject_detour::attach(GetAddress(UI::SimulatorRollover, GetRolloverIdForObject));
	TribeSpawn_detour::attach(GetAddress(Simulator, SpawnNpcTribe));

	PaletteUILoad_detour::attach(GetAddress(Palettes::PaletteUI, Load));
	PaletteUISetActiveCategory_detour::attach(GetAddress(Palettes::PaletteUI, SetActiveCategory));

	//HerdSpawn_detour::attach(GetAddress(Simulator::cGameNounManager, CreateHerd));

	// Abilities
	// TODO: needs disk spore address!
	//GetAbility_detour::attach(Address(0x00c047d0));

	// Temp? can be used to make editors read multiple palettes
	//PaletteReadProp_detour::attach(GetAddress(Palettes::PaletteMain, ReadProp));
	
}

// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

