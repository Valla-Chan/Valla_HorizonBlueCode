#include "stdafx.h"
#include "TRG_IslandEventManager.h"

/// AUTOGENERATED METHODS ///

int TRG_IslandEventManager::AddRef() {
	return Simulator::cStrategy::AddRef();
}
int TRG_IslandEventManager::Release() {
	return Simulator::cStrategy::Release();
}

const char* TRG_IslandEventManager::GetName() const {
	return "Valla_CreatureOverhaul::TRG_IslandEventManager";
}

bool TRG_IslandEventManager::Write(Simulator::ISerializerStream* stream)
{
	SporeDebugPrint("Island Event Manager is writing...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool TRG_IslandEventManager::Read(Simulator::ISerializerStream* stream)
{
	SporeDebugPrint("Island Event Manager is reading...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
}

//-----------------------------------------------------------------------------------

void TRG_IslandEventManager::Initialize() {
	SporeDebugPrint("Island Event Manager is initialized.");

	CursorManager.Load(0x24C6D844, u"cursor_unknown");
	WindowManager.GetMainWindow()->AddWinProc(this);
}

void TRG_IslandEventManager::Dispose() {
	if (mTask) {
		Simulator::RemoveScheduledTask(mTask);
		mTask = nullptr;
	}
}

void TRG_IslandEventManager::Update(int deltaTime, int deltaGameTime) {

}

//-----------------------------------------------------------------------------------

void TRG_IslandEventManager::OnModeEntered(uint32_t previousModeID, uint32_t newModeID) {
	cStrategy::OnModeEntered(previousModeID, newModeID);
	// Entered
	if (newModeID != previousModeID && newModeID == kGameTribe) {
		SporeDebugPrint("Island Event Manager is ready to use in Tribal Stage.");
		StartItemTimer();
		SpawnDummyTribe();
	}
	// Exited
	else {
		if (mTask) {
			Simulator::RemoveScheduledTask(mTask);
			mTask = nullptr;
		}
	}
}

// Generate a tribe on the "dark side" of the planet to use as a fake tribe ID for objects.
void TRG_IslandEventManager::SpawnDummyTribe() {
	// if already exists, return
	if (!IsTribeGame() || mpDummyTribe) { return; }

	// ensure the player tribe exists. if not, wait and run this again.
	auto playerTribe = GameNounManager.GetPlayerTribe();
	if (!playerTribe) { Simulator::ScheduleTask(this, &TRG_IslandEventManager::SpawnDummyTribe, 4.0f); return; }

	// find the side of the planet opposite of the player, so it is out of sight
	Vector3 pos;
	float tribeDist = Math::distance(Vector3(0, 0, 0), playerTribe->GetPosition());
	// find inverse dir
	Vector3 tribeDir = (Vector3(0, 0, 0) - playerTribe->GetPosition()).Normalized();

	// Spawn tribe
	auto species = playerTribe->mTribeMembers[0]->mpSpeciesProfile;
	mpDummyTribe = Simulator::SpawnNpcTribe(tribeDir * tribeDist, 0, 0, 0, true, species);

	// Make tribe appear destroyed, just to psych out the lore-nerds >:)
	mpDummyTribe->mpHut->mHealthPoints = mpDummyTribe->mpHut->mMaxHealthPoints / 3.0f;
	auto model = ResourceKey(id("HE_destroyed_hut"), Names::prop, id("hutstyles"));
	mpDummyTribe->mpHut->SetModelKey(model);
	mpDummyTribe->mpHut->mDestructModelHi = model;
	mpDummyTribe->mpHut->mDestructModelMd = model;
	mpDummyTribe->mpHut->mDestructModelLo = model;
	mpDummyTribe->mpHut->mUndamagedModel = model;
	// find fire pit, make it appear burned out
	for (auto tool : mpDummyTribe->mTools) {
		if (tool->GetModelKey().instanceID == id("tt_firepit_home_01")) {
			tool->SetModelKey(ResourceKey(id("tt_firepit_home_dest"), Names::prop, id("TribalTools")));
			return;
		}
	}
	
}

// Spawn a chest
void TRG_IslandEventManager::SpawnEventItem() {
	SporeDebugPrint("Spawning Event Item...");
	mbItemHovered = false;
	auto spawnPoint = FindSpawnPoint();
	if (spawnPoint && mpDummyTribe) {
		//mpEventItem = simulator_new<Simulator::cInteractiveOrnament>();
		mpEventItem = simulator_new<Simulator::cTribeHut>();

		auto model = ResourceKey(id("trg_rare_chest"), Names::prop, Models);

		mpEventItem->mpTribe = mpDummyTribe;
		const char16_t name[] = u"Treasure Chest";
		mpDummyTribe->SetName(name);

		mpEventItem->mUndamagedModel = model;
		mpEventItem->SetModelKey(model);
		mpEventItem->Teleport(spawnPoint->GetPosition(), spawnPoint->GetOrientation());
		mpEventItem->mbTransformDirty = false;
		mpEventItem->SetScale(5.0f);


		mpEventItem->mbFixed = true;
		mpEventItem->mbPickable = true;
		mpEventItem->mbIsTangible = true;
		mpEventItem->mbIsGhost = false;
	}
	if (!mpEventItem) {
		SporeDebugPrint("Spawning Failed.");
	}	

	StartItemTimer();

}

// Remove the chest
void TRG_IslandEventManager::RemoveEventItem() {
	SporeDebugPrint("Removing Event Item");
	mbItemHovered = false;
	mpEventItem->SetIsSelected(false);
	GameNounManager.DestroyInstance(mpEventItem.get());
	mpEventItem = nullptr;
	StartItemTimer();
}

// if the item is onscreen, it cannot be removed!
// call this func again in a second to check if that is still the case.
void TRG_IslandEventManager::TryRemoveEventItem() {
	if (!mpEventItem->mbInView) {
		//mpEventItem->mDistanceFromCamera
		RemoveEventItem();
	}
	else {
		SporeDebugPrint("Unable to remove Event Item.");
		mTask = Simulator::ScheduleTask(this, &TRG_IslandEventManager::TryRemoveEventItem, 1.0f);
	}
}

// Select the chest and store currently selected creatures
void TRG_IslandEventManager::SelectEventItem() {
	mpEventItem->SetIsSelected(true);
	mpEventItem->SetIsRolledOver(true);
	mpActivators.clear();
	mpActivators = GetSelectedCitizens();
}

//-----------------------------------------------------------------------------------

// TODO
void TRG_IslandEventManager::StartItemTimer() {
	float time;
	// no item, start timer to spawn one in
	if (!mpEventItem) {
		time = GetFloatFromVecRange(timeBetweenSpawns);
		mTask = Simulator::ScheduleTask(this, &TRG_IslandEventManager::SpawnEventItem, time);
	}
	// item exists, start timer to delete it.
	else {
		time = GetFloatFromVecRange(timeItemLifetime);
		mTask = Simulator::ScheduleTask(this, &TRG_IslandEventManager::TryRemoveEventItem, time);
	}
}

cSpatialObjectPtr TRG_IslandEventManager::FindSpawnPoint() {
	vector<cSpatialObjectPtr> spawnpoints = {};
	for (auto item : GetDataByCast<cSpatialObject>()) {
		if (item->GetModelKey().instanceID == id("cr_flr_coastal_large01")) { //id("TRG_lootmarker_beach")
			spawnpoints.push_back(item);
		}
	}
	if (spawnpoints.size() > 0) {
		return spawnpoints[rand(spawnpoints.size() - 1)];
	}
	return nullptr;
}

// TODO
ResourceKey TRG_IslandEventManager::GetRandResource(uint32_t groupID) const {
	return ResourceKey();
}

float TRG_IslandEventManager::GetFloatFromVecRange(Vector2 range) const {
	float diff = range.y - range.x;
	return (randf() * diff) + range.x;
}

vector<cCreatureCitizenPtr> TRG_IslandEventManager::GetSelectedCitizens() const {
	vector<cCreatureCitizenPtr> selectedCreatures = {};
	auto tribe = GameNounManager.GetPlayerTribe();
	if (!tribe) { return selectedCreatures; }
	for (auto creature : tribe->GetTribeMembers()) {
		if (creature->IsSelected()) {
			selectedCreatures.push_back(creature);
		}
		
	}
	return selectedCreatures;
}

bool TRG_IslandEventManager::IsEventItemHovered() const {
	if (mpEventItem) {
		return mpEventItem->IsRolledOver();
	}
	return false;
}

/*
bool TRG_IslandEventManager::TraceCanHandleModel(Graphics::Model* model) {
	if (object_cast<cGamePlant>(model->mpOwner)) { return false; }
	return true;
}

bool TRG_IslandEventManager::TraceHitEventObject() const {

	auto pViewer = CameraManager.GetViewer();

	Vector3 camPos = Vector3(0, 0, 0);
	Vector3 camDir = Vector3(0, 0, 0);

	// Get vector to the center of the screen.
	auto windowArea = WindowManager.GetMainWindow()->GetArea();
	pViewer->GetCameraToMouse(camPos, camDir);

	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			// if hit a tribal building before an object, return false.
			//if (object_cast<cTribeTool>(object) || object_cast<cTribeHut>(object)) {
			//	return false;
			//}
			//auto worldObject = object_cast<cInteractiveOrnament>(object);
			auto worldObject = object_cast<cTribeHut>(object);
			if (worldObject && worldObject == mpEventItem) {
				return true;
			}
		}

	}
	return false;
}*/

//-----------------------------------------------------------------------------------


int TRG_IslandEventManager::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

bool TRG_IslandEventManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsTribeGame()) { return false; }
	if (GetSelectedCitizens().size() == 0) { return false; }
	if (!mpEventItem) { mbItemHovered = false; return false; }

	if (message.Mouse.IsRightButton()) {
		// Clicked right mouse btn, start registering the click and items.
		mbCameraMoved = false;
		if (message.IsType(kMsgMouseDown) && !mbItemHovered) {
			if (IsEventItemHovered()) {
				mbItemHovered = true;
				return false;
			}
		}
	}
	// Clicked Left mouse btn OR
	// Moved mouse with button held and item hovered. clear the state.
	if ( (message.Mouse.IsLeftButton() || message.IsType(kMsgMouseMove)) && mbItemHovered) {
		mbCameraMoved = true;
		mbItemHovered = false;
		// do not allow left clicking on this item, to prevent showing of fake tribe.
		if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseDown) && IsEventItemHovered()) {
			return true;
		}
		return false;
	}
	
	// Released mouse button without moving it (allow selecting now!)
	if (message.IsType(kMsgMouseUp)) {
		bool rolled = IsEventItemHovered();
		// If item is hovered and the bool is set, select it and begin special behavior.
		if (rolled && mbItemHovered && !mbCameraMoved) {
			SelectEventItem();
		}
		// If not rolled over the object and not moving the camera, the player is directing the creature to a new task,
		// so remove these creatures from the activators list.
		else if (!rolled && !mbCameraMoved) {
			auto selected = GetSelectedCitizens();
			// loop thru current activators to find matches
			for (size_t i = 0; i < mpActivators.size(); i++) {
				// check this activator against all the selected creatures, and set activator entry to null if a match is found.
				for (auto selected : selected) {
					if (mpActivators[i] == selected) {
						mpActivators[i] = nullptr;
						continue;
					}
				}
			}
		}
		mbItemHovered = false;
		return false;
	}


	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}

//-----------------------------------------------------------------------------------


Simulator::Attribute TRG_IslandEventManager::ATTRIBUTES[] = {
	// Add more attributes here
	SimAttribute(TRG_IslandEventManager,mpActivators,0),
	SimAttribute(TRG_IslandEventManager,mpEventItem,1),
	SimAttribute(TRG_IslandEventManager,mpDummyTribe,2),
	// This one must always be at the end
	Simulator::Attribute()
};


// You can extend this function to return any other types your class implements.
void* TRG_IslandEventManager::Cast(uint32_t type) const
{
	//CLASS_CAST(Simulator::cStrategy);
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(TRG_IslandEventManager);
	return nullptr;
}