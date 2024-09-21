#pragma once

#include <Spore\BasicIncludes.h>
//#include <Spore\UTFWin\Message.h>

#define EP1_GameplayObjectPtr intrusive_ptr<EP1_GameplayObject>

using namespace Simulator;

class EP1_GameplayObject 
	: public Object
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
	// Vars Structs and Enums

	enum Mode {
		Unk1,
		EditMode,
		PlayMode,
	};

	// currently there can only be one activator at a time.
	struct GameplayObject {
		cSpatialObjectPtr object = nullptr;
		cCombatantPtr activator = nullptr;
		bool active = true;
		// TODO: add in a way to check if a cSpatialObjectPtr object is active
		// and create a cooldown for when object is triggered, to turn off active and turn on again after it is over.

		GameplayObject(cSpatialObjectPtr p_object = nullptr, cCombatantPtr p_activator = nullptr, bool p_active = true) {
			object = p_object;
			activator = p_activator;
			active = p_active;
		}

		void clear() {
			object = nullptr;
			activator = nullptr;
			active = false;
		}
	};

	// vector of object structs to use when ingame
	eastl::vector<GameplayObject> mpIngameObjects = {};

	// Currently selected/edited object. Can be creature or gameplay object.
	cSpatialObjectPtr pHeldObject = nullptr;

	cCombatantPtr GetHeldCombatant() { return object_cast<cCombatant>(pHeldObject); }
	cCombatantPtr GetHeldCreature() { return object_cast<cCreatureAnimal>(pHeldObject); }

	bool mbExcludeAvatar = true;
	bool mbCheckRadiusPerFrame = false;

	bool mbReapplyEffect = false;

	bool mbCombatantCanBeCreature = true;
	bool mbCombatantCanBeVehicle = false;
	bool mbCombatantCanBeBuilding = false;
	bool mbCombatantCanBeMisc = false;
	

	//------------------------------------------------------------
	
	// States
	static bool IsPlayingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode); }
	static bool IsEditingAdventure() { return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::EditMode); }
	static auto Avatar() { return GameNounManager.GetAvatar(); }
	bool IsObjectHeld(cSpatialObjectPtr object) { return object == pHeldObject; }

	// Get Object Pointers
	eastl::vector<cSpatialObjectPtr> GetAllObjects(); // all handled objects
	void StoreObjects(); // GetAllObjects() and store them in mpIngameObjects
	tGameDataVectorT<cCombatant> GetCombatantData() const;

	cSpatialObjectPtr GetClosestHandledObject(cSpatialObjectPtr srcobject); // closest handled object (to a src object)
	virtual cCombatantPtr GetClosestCombatant(cSpatialObjectPtr srcobject); // closest creature, vehicle, etc (to a src object)
	cSpatialObjectPtr GetRolledHandledObject();
	cCombatantPtr GetRolledCombatant();

	GameplayObject GetIngameObject(cSpatialObjectPtr srcobject);

	// Helper funcs
	void InternalUpdate();
	void CheckRadius();
	// Input Precursors - Editor
	bool DoPickup();
	bool DoDrop();
	bool DoMoved();
	// Damage precursor
	void DoTakeDamage(cCombatantPtr target, float damage, cCombatantPtr attacker);

	//------------------------------------------------------------
	// Virtuals - Object Definition

	// Returns if an object is one that is handled by this class.
	virtual bool IsHandledObject(cSpatialObjectPtr object) const = 0; //{ return false; }

	// Returns if a combatant is of the valid types
	virtual bool IsValidCombatantType(cCombatantPtr object);


	// Returns the max radius of influence of an object. Often multiplies by the scale of the object.
	virtual float GetObjectMaxRadius(cSpatialObjectPtr object);

	// Returns the reference pos an object. Can be overridden to include an offset.
	virtual Vector3 GetObjectPos(cSpatialObjectPtr object);

	// Returns the abs(distance) if object A is in range of object B.
	// Defaults to a double distance check; can be overriden to check anything, such as origin within bbox or etc.
	// if not within range, return -1.
	virtual float IsObjectInRange(cSpatialObjectPtr objectA, cSpatialObjectPtr objectB);

	void Destroy(cSpatialObjectPtr object);

	//------------------------------------------------------------
	// Virtuals - External Actions

	// fired when damage is taken on a handled object
	virtual void OnDamaged(cCombatantPtr object, float damage, cCombatantPtr pAttacker);
	// fired when damage is taken on a handled object's activator
	virtual void OnActivatorDamaged(cCombatantPtr object, float damage, cCombatantPtr pAttacker);

	// radius refers to the value returned by GetObjectMaxRadius()
	virtual void OnEnterRadius(cSpatialObjectPtr object, cCombatantPtr pActivator);
	virtual void OnExitRadius(cSpatialObjectPtr object, cCombatantPtr pActivator);


	//------------------------------------------------------------
	// Virtuals - Actions from Manager

	// Apply desired behavior to all objects.
	void ApplyAllObjectEffects();

	// Tell the object to apply its effect.
	virtual void ApplyObjectEffect(cSpatialObjectPtr object);
	// Apply effect from an object onto a creature ('object' ptr can be used for getting offsets, colors, etc)
	virtual void ApplyCombatantEffect(cCombatantPtr combatant, cSpatialObjectPtr object);
	virtual void ResetCombatantEffect(cCombatantPtr combatant);


	// Inputs - Editor
	virtual bool Pickup();
	virtual bool Drop();
	virtual bool Moved();
	// Inputs - Playmode
	virtual bool MouseClick() { return false; }
	virtual bool MouseRelease() { return false; }
	virtual bool MouseDrag() { return false; }

	// Messages
	virtual void Update();
	virtual void UndoRedo();
	virtual void SwitchGameMode(bool to_scenario);
	virtual void EnterMode(int mode);
	virtual void UpdateScenarioGoals();

	virtual bool UserUIMessage(UTFWin::Message& message) { return false; }
	virtual bool UserGameMessage(uint32_t messageID) { return false; }

	//------------------------------------------------------------
	
private:
	bool mbHeldObjectApplied = false;
	
};
