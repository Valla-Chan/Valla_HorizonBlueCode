#pragma once

#include <Spore\BasicIncludes.h>

#define TRG_IslandEventManagerPtr intrusive_ptr<TRG_IslandEventManager>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;
using namespace TypeIDs;

class TRG_IslandEventManager 
	: public Simulator::cStrategy
	, public IWinProc
{
public:
	static const uint32_t TYPE = id("Valla_CreatureOverhaul::TRG_IslandEventManager");
	static const uint32_t NOUN_ID = TYPE;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t typeID) const override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;
	//-------------------------------------------------------------------------------------

	/// This class generates item chests on the beach at certain points determined by a game noun spawned by creature nests.
	/// Only 1 event item will be active in the world at once, and it will only appear for a limited amount of time before it disappears again.
	/// Event items will NOT disappear if they are onscreen.
	/// 
	/// Clicking on this item will direct any selected tribe member to its location, and select the item visually.
	/// Store the curretly selected tribe members in a vector of Ptrs (mpActivators; clear data then store.) that can be validated against,
	/// and when any of those creatures are within range of the item, have them play an animation of digging bones.
	/// 
	/// If any of the creatures in the Activators vector are selected when right clicking somewhere else (event item is not hovered),
	/// remove them from the mpActivators list (set entry to nullptr)
	/// 
	/// After a point in the dig anim has been hit, change the chest model (or etc) to open, freeze the game, and pop up a UI with an event prompt.

	const Vector2 timeBetweenSpawns = Vector2(1.0f, 4.0f);
	const Vector2 timeItemLifetime = Vector2(20.0f, 25.0f);

	enum Paths {
		Models = id("IslandEvents_Models"),
		Prompts = id("IslandEvents_Prompts"),
		Results = id("IslandEvents_Results"),
		EditorRigblock = 0x40606000,
	};

	vector<cCreatureCitizenPtr> mpActivators = {}; // creatures that have been set to investigate the item
	//cInteractiveOrnamentPtr mpEventItem = nullptr;
	cTribeHutPtr mpEventItem;
	cTribePtr mpDummyTribe; // this could be moved to a more generic tribe manager class if other classes want to access it

	bool mbItemHovered = false; // if this is true, releasing the mouse button while hovering the object will select it.
	bool mbCameraMoved = false; // if this is true, the camera has moved, and releasing the mouse will not direct a tribe member.

	//--------------------------------------------------------------------

	void OnModeEntered(uint32_t previousModeID, uint32_t newModeID) override;

	void SpawnDummyTribe();
	void SpawnEventItem();
	void RemoveEventItem();
	void TryRemoveEventItem();
	void SelectEventItem();

	// Start timer. If the item exists, timer to despawn. If it does not exist, timer to spawn it.
	void StartItemTimer();
	// loop through a list of objects in the world that function as spawners, and get a position from one of them.
	cSpatialObjectPtr FindSpawnPoint();
	// Get a random resource from a directory
	ResourceKey GetRandResource(uint32_t groupID) const;
	// get a random float from a vector range
	float GetFloatFromVecRange(Vector2 range) const;

	vector<cCreatureCitizenPtr> GetSelectedCitizens() const;
	bool IsEventItemHovered() const;

	//static bool TraceCanHandleModel(Graphics::Model* model);
	//bool TraceHitEventObject() const;

	//--------------------------------------------------------------------
	
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	

	static Simulator::Attribute ATTRIBUTES[];
	
private:
	// This task can be stopped and reset to a new time
	intrusive_ptr<Simulator::ScheduledTaskListener> mTask;
};