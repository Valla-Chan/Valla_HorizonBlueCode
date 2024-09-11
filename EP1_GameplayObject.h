#pragma once

#include <Spore\BasicIncludes.h>

#define EP1_GameplayObjectPtr intrusive_ptr<EP1_GameplayObject>

using namespace UTFWin;
using namespace Simulator;

class EP1_GameplayObject 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObject");
	
	EP1_GameplayObject();
	~EP1_GameplayObject();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;


	static bool IsPlayingAdventure();
	static bool IsEditingAdventure();

	cSpatialObjectPtr pSelectedObject = nullptr;

	static cCreatureAnimalPtr GetClosestCreature(cSpatialObjectPtr gameplayob, float max_distance, bool exclude_avatar = false);
	static cSpatialObjectPtr GetRolledObject();
	static cCreatureAnimalPtr GetRolledCreature();

	virtual void Pickup() = 0;
	virtual void Drop() = 0;
	virtual void Moved() = 0;

	// Default Message Behaviors
	bool HandleMouseInput(const Message& message); // calls Pickup, Drop, and Moved
	void UserMessage(uint32_t messageID);
	void UndoRedo();
	void SwitchMode();

	int GetEventFlags() const override;
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;
	void FireIfScenario();
};
