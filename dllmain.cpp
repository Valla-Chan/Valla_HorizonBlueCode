// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

// Cheats
#include "Cheats/BuildCRG.h"
#include "Cheats/Kill.h"
#include "Cheats/CRG_Starve.h"
#include "Cheats/CRG_BrainBoost.h"
#include "Cheats/CRG_BrainDrain.h"
#include "Cheats/CRG_SwapCreature.h"
#include "Cheats/CRG_PosessTarget.h"
#include "Cheats/CRG_ResurrectTarget.h"
#include "Cheats/CRG_Recharge.h"
#include "Cheats/CRG_SpawnPlant.h"
#include "Cheats/CRG_GrowUp.h"

// Ingame Behaviors
#include "CRG_EnergyHungerSync.h"
#include "CRG_WaterBehavior.h"


// Meta Behaviors
//#include "LOAD_LoadingSpeed.h"

// Scripts
//#include "CRG_ObjectConverter.h"

// Singletons
#include "CapabilityChecker.h"
#include "CRG_ObjectManager.h"

cObjectManager* obconverter;

void Initialize()
{
	// This method is executed when the game starts, before the user interface is shown
	CheatManager.AddCheat("Build", new(BuildCRG));
	CheatManager.AddCheat("Kill", new(Kill));
	CheatManager.AddCheat("Starve", new(CRG_Starve));
	CheatManager.AddCheat("BrainBoost", new(CRG_BrainBoost));
	CheatManager.AddCheat("BrainDrain", new(CRG_BrainDrain));
	//CheatManager.AddCheat("SwapCreature", new(CRG_SwapCreature));
	CheatManager.AddCheat("PosessTarget", new(CRG_PosessTarget));
	CheatManager.AddCheat("ResurrectTarget", new(CRG_ResurrectTarget));
	CheatManager.AddCheat("Recharge", new(CRG_Recharge));
	CheatManager.AddCheat("SpawnPlant", new(CRG_SpawnPlant));
	CheatManager.AddCheat("GrowUp", new(CRG_GrowUp));

	// TODO: these would be better to only attach upon entering creature stage.
	// (I dont know how to do that yet.)
	CRG_EnergyHungerSync* energyhungersync = new(CRG_EnergyHungerSync);
	CRG_WaterBehavior* waterbehavior = new(CRG_WaterBehavior);

	// Object Converter
	obconverter = new(cObjectManager);
	//MessageManager.AddListener(obconverter, Simulator::kMsgGameNounStatusChanged);

	// Singletons
	cCapabilityChecker* capchecker = new(cCapabilityChecker);
}


// Detour the animation playing func
virtual_detour(AnimOverride_detour, Anim::AnimatedCreature, Anim::AnimatedCreature, void(uint32_t, int*)) {
	void detoured(uint32_t animID, int* pChoice) {

		if (IsCreatureGame()) {
			cCreatureAnimal* avatar = GameNounManager.GetAvatar();
			if (avatar && animID == 0x03DF6DFF) { //gen_dig_hands

				cInteractiveOrnament* object = ObjectManager.FindInteractedObject();
				obconverter->SetInteractedObject(object);

				ResourceKey animkey = ObjectManager.GetModelInteractAnim(object->GetModelKey());

				if (animkey.instanceID != 0x0) {
					//obconverter->waiting_for_noun = true;
					obconverter->StartWaitingForNoun();
					original_function(this, animkey.instanceID, pChoice); // 0x04F65995, eat_meat_mouth_01
					return;
				}
				
			}
		}
		original_function(this, animID, pChoice);

	}
};


void Dispose()
{
	obconverter = nullptr;
	// This method is called when the game is closing
}

void AttachDetours()
{
	AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
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

