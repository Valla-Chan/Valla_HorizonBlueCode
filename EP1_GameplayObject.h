#pragma once

#include <Spore\BasicIncludes.h>

#define EP1_GameplayObjectPtr intrusive_ptr<EP1_GameplayObject>

using namespace UTFWin;
using namespace Simulator;

class EP1_GameplayObject 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("EP1_GameplayObject");
	
	EP1_GameplayObject();
	~EP1_GameplayObject();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

	//------------------------------------------------------------
	// Vars

	// Can be creature or gameplay object.
	cSpatialObjectPtr pSelectedObject = nullptr;

	//------------------------------------------------------------
	
	// States
	static bool IsPlayingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode); }
	static bool IsEditingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::EditMode); }

	// Get Object Pointers
	static eastl::vector<cSpatialObjectPtr> GetAllObjects(); // all handled objects
	static cSpatialObjectPtr GetClosestObject(cCreatureAnimalPtr creature, bool exclude_avatar = false); // closest handled object
	static cCreatureAnimalPtr GetClosestCreature(cSpatialObjectPtr object,  bool exclude_avatar = false); // closest creature
	static cSpatialObjectPtr GetRolledObject();
	static cCreatureAnimalPtr GetRolledCreature();


	//------------------------------------------------------------
	// Virtuals - Object Definition

	// returns if an object is one that is handled by this class.
	virtual bool IsHandledObject(cSpatialObjectPtr object);

	// returns the max radius of influence of an object. Often multiplies by the scale of the object.
	virtual float GetObjectMaxRadius(cSpatialObjectPtr object);


	//------------------------------------------------------------
	// Virtuals - Actions from Manager

	// Apply desired behavior to all objects.
	void ApplyAllObjectEffects();

	// tell the object to apply its effect.
	virtual void ApplyObjectEffect(cSpatialObjectPtr object) = 0;
	// apply effect from an object onto a creature ('object' ptr can be used for getting offsets, colors, etc)
	virtual void ApplyCreatureEffect(cCreatureAnimalPtr creature, cSpatialObjectPtr object) = 0;


	// Inputs
	virtual bool Pickup() { return false; }
	virtual bool Drop() { return false; }
	virtual bool Moved() { return false; }

	// Messages
	virtual void Update() { return; };
	virtual void UndoRedo();
	virtual void SwitchMode(bool to_scenario);

	virtual bool UserUIMessage(Message& message) { return false; }
	virtual bool UserGameMessage(uint32_t messageID) { return false; }

	//------------------------------------------------------------
	
	
};
