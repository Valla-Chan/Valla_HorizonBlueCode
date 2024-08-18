// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "BuildCRG.h"

// Cheats
#include "Kill.h"
#include "CRG_BrainBoost.h"
#include "CRG_BrainDrain.h"
#include "CRG_SwapCreature.h"
#include "CRG_PosessTarget.h"
#include "CRG_ResurrectTarget.h"
#include "CRG_Recharge.h"
#include "CRG_SpawnPlant.h"

// Ingame Behaviors
#include "CRG_EnergyHungerSync.h"
#include "CRG_WaterBehavior.h"

// Meta Behaviors
//#include "LOAD_LoadingSpeed.h"

// Singletons
#include "CapabilityChecker.h"

void Initialize()
{
	// This method is executed when the game starts, before the user interface is shown
	CheatManager.AddCheat("Build", new(BuildCRG));
	CheatManager.AddCheat("Kill", new(Kill));
	CheatManager.AddCheat("BrainBoost", new(CRG_BrainBoost));
	CheatManager.AddCheat("BrainDrain", new(CRG_BrainDrain));
	//CheatManager.AddCheat("SwapCreature", new(CRG_SwapCreature));
	CheatManager.AddCheat("PosessTarget", new(CRG_PosessTarget));
	CheatManager.AddCheat("ResurrectTarget", new(CRG_ResurrectTarget));
	CheatManager.AddCheat("Recharge", new(CRG_Recharge));
	CheatManager.AddCheat("SpawnPlant", new(CRG_SpawnPlant));

	// TODO: these would be better to only attach upon entering creature stage.
	// (I dont know how to do that yet.)
	CRG_EnergyHungerSync* energyhungersync = new(CRG_EnergyHungerSync);
	CRG_WaterBehavior* waterbehavior = new(CRG_WaterBehavior);

	//
	//LOAD_LoadingSpeed* loadingspeed = new(LOAD_LoadingSpeed);
	//MessageManager.AddListener(loadingspeed, Simulator::kMsgSwitchGameMode); //make the Ability Manager function

	//
	cCapabilityChecker* capchecker = new(cCapabilityChecker);
}

void Dispose()
{
	// This method is called when the game is closing
}

void AttachDetours()
{
	// Call the attach() method on any detours you want to add
	// For example: cViewer_SetRenderType_detour::attach(GetAddress(cViewer, SetRenderType));
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

