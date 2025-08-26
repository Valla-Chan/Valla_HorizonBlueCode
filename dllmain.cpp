// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore/Effects.h>
#include <Spore/Swarm/cEffectsManager.h>
#include <Spore\Simulator\cCreatureGameData.h>

// Cheats
#include "Cheats/HBCheats.h"

// UI
#include "UI_Timescale.h"
#include "UI_RenamePlanet.h"

// Editor
#include "HBAssetBrowser.h"

// Stages
#include "HBCell.h"
#include "HBCrg.h"
#include "HBTribe.h"
#include "HBCiv.h"
#include "HBSpace.h"
#include "HBEp1.h"


// Singletons
#include "CapabilityChecker.h"

// Scripts
#include "CreatureSpeedBoost.h"

// Game Modes
#include "ArenaMode.h"

void Initialize()
{
	// Arena Mode
	auto arena = new ArenaMode();
	GameModeManager.AddGameMode(arena, ArenaMode::MODE_ID, "ArenaMode");

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

		// DEBUG:
//#ifdef DEBUG
		if (GameNounManager.GetPlayerTribe() ) {
			
			auto creature = Common::GetAnimCreatureCitizenOwner(this);
			// only check anims on the chieftain!
			//if (GameNounManager.GetPlayerTribe()->GetLeaderCitizen() == creature) {

				// only check anims on the player tribe members
				if (creature && creature->mpOwnerTribe == GameNounManager.GetPlayerTribe()) {

					//SporeDebugPrint("Animation: %x", animID);
				}
			//}
			
		}
		auto hovered = GameViewManager.GetHoveredObject();
		auto hoveredAnimal = object_cast<cCreatureAnimal>(hovered);
		if (hoveredAnimal) {
			auto creature = Common::GetAnimCreatureOwner(this);
			if (creature == hoveredAnimal) {
				//SporeDebugPrint("Hovered Creature Animation: %x", animID);
			}
			
		}

//#endif // DEBUG	
		
		//SporeDebugPrint("Animation: %x", animID);
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



// Detour the herd spawning func
// TODO: NOT CURRENTLY USED
member_detour(HerdSpawn_detour, Simulator::cHerd, cHerd*(const Vector3&, cSpeciesProfile*, int, bool, int, bool)) {
	cHerd* detoured(const Vector3 & position, cSpeciesProfile* pSpeciesProfile, int herdSize, bool isOwnedByAvatar, int creaturePersonality, bool createNest)
	{
		//App::ConsolePrintF("Herd Spawned");
		cHerd* herd = original_function(this, position, pSpeciesProfile, herdSize, isOwnedByAvatar, creaturePersonality, createNest);
		return herd;

		// TODO: herd variety
	}
};

//-----------------------------------


// Detour the effect playing func
member_detour(EffectOverride_detour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId) // Detour the function for obtaining effect indexes
	{
		// detect if a cinematic is beginning and emit a message
		if (instanceId == id("fade_to_black_1") || instanceId == id("fade_to_black_3") || instanceId == id("fade_to_black_quick")) {
			MessageManager.MessageSend(id("CinematicBegin"), nullptr);
		}

		// detect if a baby is growing up and send a message or suppress FX.
		if (instanceId == 0x3A616FEE) {
			MessageManager.MessageSend(id("BabyGrowUp"), nullptr);
		}

		// Print FX
		//if (instanceId != 0x0) { SporeDebugPrint("%x", instanceId); }

		return original_function(this, instanceId, groupId); // Call the original function with the new instance ID.
	}
};


/*
virtual_detour(GetAbility_detour, Simulator::cCreatureCitizen, Simulator::cCreatureBase, cCreatureAbility* (int))
{
	cCreatureAbility* detoured(int index)
	{
		// if using a new tool, only allow this creature to use this ability.
		if (this->mSpecializedTool > HomeEnd) {
			
		}
		return original_function(this, index);
	}
};

virtual_detour(CitizenUpdate_detour, Simulator::cCreatureCitizen, Simulator::cCreatureBase, void (int))
{
	void detoured(int deltaTime)
	{
		original_function(this, deltaTime);
		if (this->mSpecializedTool > HomeEnd) {
			TribeToolManager.SetDesiredHandheldItem(this);
		}
	}
};*/


/*
// adding window to another window
// TODO: NOT CURRENTLY USED
virtual_detour(AddWindow_detour, UTFWin::Window, UTFWin::Window, void(IWindow*)) {
	void detoured(IWindow * pWindow) {
		original_function(this, pWindow);
	}
};

// TODO: NOT CURRENTLY USED
member_detour(PlayModeSetWindowVisible_detour, Editors::PlayModeUI, void(uint32_t, bool)) {
	void detoured(uint32_t controlID, bool visible) {
		original_function(this, controlID, visible);
		
		if (controlID) {
			auto parent = WindowManager.GetMainWindow()->FindWindowByID(0x0445A468);
			UILayout buttonloadcrt;
				
			if (parent && !parent->FindWindowByID(0x41F745F1)) {
				buttonloadcrt.LoadByID(id("editor_button_loadcreature"));
				parent->AddWindow(buttonloadcrt.FindWindowByID(0x0445A469));
			}
		}
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
*/

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

	//HerdSpawn_detour::attach(GetAddress(Simulator::cGameNounManager, CreateHerd));

	// Abilities
	// TODO: needs disk spore address!
	//GetAbility_detour::attach(Address(0x00c047d0));
	//CitizenUpdate_detour::attach(GetAddress(Simulator::cCreatureBase, Update));

	//AddWindow_detour::attach(GetAddress(UTFWin::Window, AddWindow));
	//PlayModeSetWindowVisible_detour::attach(GetAddress(PlayModeUI, SetWindowVisible));

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

