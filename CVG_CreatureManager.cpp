#include "stdafx.h"
#include "CVG_CreatureManager.h"
#include <Spore\App\cMouseCamera.h>
#include <Spore\Simulator\cSpaceNames.h>
#include <Spore\Sporepedia\AssetBrowserQuery.h>
#include <Spore\Sporepedia\ObjectTemplateDB.h>
#include "CapabilityChecker.h"

/// AUTOGENERATED METHODS ///

int CVG_CreatureManager::AddRef() {
	return Simulator::cStrategy::AddRef();
}
int CVG_CreatureManager::Release() {
	return Simulator::cStrategy::Release();
}
// You can extend this function to return any other types your class implements.
void* CVG_CreatureManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(CVG_CreatureManager);
	return nullptr;
}

const char* CVG_CreatureManager::GetName() const {
	return "Valla_CreatureOverhaul::CVG_CreatureManager";
}

bool CVG_CreatureManager::Write(Simulator::ISerializerStream* stream)
{
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool CVG_CreatureManager::Read(Simulator::ISerializerStream* stream)
{
	used_creature_indexes.clear();
	auto value = Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
	if (IsCivGame()) {
		Simulator::ScheduleTask(this, &CVG_CreatureManager::NewCityAppeared, 0.1f);
	}
	return value;
}

/// END OF AUTOGENERATED METHODS ///
////////////////////////////////////

Simulator::Attribute CVG_CreatureManager::ATTRIBUTES[] = {
	// Add more attributes here
	//SimAttribute(CVG_CreatureManager, mClosestCityCreaturePositions, 1),
	//SimAttribute(CVG_CreatureManager, mTimeOfDay, 1),
	// This one must always be at the end
	Simulator::Attribute()
};

void CVG_CreatureManager::Initialize() {
	//MessageManager.AddListener(this, kMsgCinematicNewCityAppears);
	WindowManager.GetMainWindow()->AddWinProc(this);
}

void CVG_CreatureManager::Dispose() {
	
}

void CVG_CreatureManager::Update(int deltaTime, int deltaGameTime) {

	if (!IsCivGame()) { return; }

	if (IsPlayerCityInRange()) {
		
		cCreatureCitizenPtr citizenHovered = GetHoveredCitizen();

		// citizen found, show rollover
		if (citizenHovered) {
			// if the creature has no name, generate one.
			if (citizenHovered->mCreatureName.empty()) {
				citizenHovered->mCreatureName = SpaceNames.GenerateRandomName(SpaceNamesType::kCreature);
			}
			SimulatorRollover::ShowRollover(citizenHovered.get());
		}
	}
	
}

//---------------------------------------------------------------------------

bool CVG_CreatureManager::IsPlayerCityInRange() const {
	auto pViewer = CameraManager.GetViewer();
	// get camera distance from planet
	float cameraDist = 0;
	{
		Vector3 cameraPos;
		Vector3 cameraDir;
		pViewer->GetCameraToMouse(cameraPos, cameraDir);
		cameraDist = cameraPos.Length();
	}
	// detect camera dist, below ~600 should suffice for close range.
	if (cameraDist < 600) {
		return true;
	}
	return false;
}

// Player-owned city hovered
bool CVG_CreatureManager::IsPlayerCityHovered() const {
	if (IsPlayerCityInRange()) {
		auto window = WindowManager.GetMainWindow();
		auto cityHover = window->FindWindowByID(0x0199A7DE); // find the city hover UI
		IWindow* cityHoverEdit = nullptr;

		if (cityHover) {
			cityHoverEdit = cityHover->FindWindowByID(0x062D5618); // find the city hover UI for players to see if player city is hovered
		}

		// Player-owned city hovered
		if (cityHoverEdit && cityHoverEdit->IsVisible()) {
			return true;
		}
	}
	return false;
}

// combine traces and hover
cCreatureCitizenPtr CVG_CreatureManager::GetHoveredCitizen(bool playerowned) const {
	cCreatureCitizenPtr citizenHovered = nullptr;
	if (IsPlayerCityInRange() || !playerowned) {

		if (IsPlayerCityHovered() || !playerowned) {
			citizenHovered = TraceHitCitizen();
		}
		// No city hovered, fall back to actual rollover
		else if (GameViewManager.mHoveredObject) {
			auto citizen = object_cast<cCreatureCitizen>(GameViewManager.mHoveredObject);
			if (citizen && citizen->mpOwnerCity && citizen->mpOwnerCity->IsPlayerOwned()) {
				citizenHovered = citizen;
			}
		}
	}
	return citizenHovered;
}

cCreatureCitizenPtr CVG_CreatureManager::TraceHitCitizen() const {

	auto pViewer = CameraManager.GetViewer();

	Vector3 camPos = Vector3(0, 0, 0);
	Vector3 camDir = Vector3(0, 0, 0);

	// Get vector to the center of the screen.
	auto windowArea = WindowManager.GetMainWindow()->GetArea();
	pViewer->GetCameraToMouse(camPos, camDir);

	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto citizen = object_cast<cCreatureCitizen>(object);
			if (citizen && citizen->mpOwnerCity->IsPlayerOwned()) {
				return citizen;
			}
		}
	}
	return nullptr;
}

bool CVG_CreatureManager::IsCreatureIndexUsed(int index) const {
	for (auto item : used_creature_indexes) {
		if (index == item) {
			return true;
		}
	}
	return false;
}

// Randomize the tribe species based on tribe/civ species, or regular animals as backup.
// Allow chance for player species to still spawn,
// and eliminate used creature key indexes.
// TODO: pull from the living NPC tribe races at the end of tribal stage before randomizing.

cSpeciesProfile* CVG_CreatureManager::GetRandomTribeSpecies(cSpeciesProfile* species) {
	// returns the original species based on a random chance.
	if (randf(0, 1.0f) <= mPlayerSpeciesChance) {
		return species;
	}

	vector<OTDB::QueryParameter> params;
	// Only get creations with these params:
	if (!found_all_tribe_creatures) {
		params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::totalAttack, 2.0f, 30.0f)); // 7
		params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::totalSocial, 2.0f, 30.0f)); // 3
		//params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::height, 1.0f, 200.0f));
	}
	else {
		params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::totalAttack, 4.0f, 30.0f));
		params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::totalSocial, 3.0f, 30.0f));
		//params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::height, 3.0f, 200.0f));

		//params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::numFeet, 1.0f, 4.0f));
		//params.push_back(OTDB::QueryParameter::Float(OTDB::Parameters::numGraspers, 1.0f, 4.0f));
	}
	
	vector<ResourceKey> keys;
	ObjectTemplateDB.FindObjects(keys, params);

	int current_try = 0;
	ResourceKey key;
	int index = -1;

	// give the key randomizer this many randomization tries before continuing with whatever it found.
	int tries = 300;
	while (current_try < tries) {
		
		current_try++;

		// randomize a new key index
		index = rand(keys.size());

		// if this index has already been used, skip it and re-randomize.
		int indexTries = 0;
		while (IsCreatureIndexUsed(index)) {
			if (!found_all_tribe_creatures && (indexTries > 20 || current_try > 299)) {
				found_all_tribe_creatures = true;
				current_try = 0;
				indexTries = 0;
			}
			else {
				index = rand(keys.size());
				indexTries++;
			}
		}

		// set key to valid index.
		if (index < int(keys.size())) {
			key = keys[index];
		}
		else {
			return species;
		}
		

		// begin reviewing asset data of chosen item
		cSPAssetDataOTDB* assetdata1 = new(cSPAssetDataOTDB);
		assetdata1->SetKey(key);

		auto subtype = assetdata1->GetAssetSubtype();
		// do not allow cells
		if (subtype != ModelTypes::kCell) {
			

			// if tribe or civ creature, use this key for the tribe.
			if (subtype == ModelTypes::kTribeCreature || subtype == ModelTypes::kCivCreature) {
				current_try = tries;
				used_creature_indexes.push_back(index);
			}
			// reached end of list, start pulling backup creatures instead.
			else if (current_try > tries - 1) {
				found_all_tribe_creatures = true;
				current_try = 0;
			}

			if (found_all_tribe_creatures) {
				// if using a backup creature, just make sure it isnt too advanced.
				if (subtype != ModelTypes::kAdventurerCreature &&
					subtype != ModelTypes::kSpaceCreature) {

					// make sure it has hands and feet, unless its getting close to the last few tries.
					if (current_try > 250 ||
						// grasper and foot capabilities:
						(CapabilityChecker.GetCapabilityLevelFromResource(key, 0xB00F0FE2) > 0 &&
							CapabilityChecker.GetCapabilityLevelFromResource(key, 0xB00F0FEF) > 0)) {

						current_try = tries;
						used_creature_indexes.push_back(index);
					}
				}
			}
			
		}
		// if the found asset was a cell, give the randomizer another try.
		else {
			current_try -= 1;
			used_creature_indexes.push_back(index);
		}
	}

	return SpeciesManager.GetSpeciesProfile(key);

}


int CVG_CreatureManager::DetermineNeededCityType() const {
	int cityTypes1 = 0;
	int cityTypes2 = 0;
	int cityTypes3 = 0;

	auto cities = GetData<cCity>();
	for (auto item : cities) {
		switch (item->mVehicleSpecialty) {
		case 1: cityTypes1++; break;
		case 2: cityTypes2++; break;
		case 3: cityTypes3++; break;
		}
	}

	float thresholdScalar = 0.28f;
	// one of the values is zero, might want to spawn that type next.
	if (cityTypes1 == 0 || cityTypes1 < ceil((cityTypes2 + cityTypes3) * thresholdScalar)) {
		return 0;
	}
	else if (cityTypes2 == 0 || cityTypes2 < ceil((cityTypes1 + cityTypes3) * thresholdScalar)) {
		return 1;
	}
	else if (cityTypes3 == 0 || cityTypes3 < ceil((cityTypes1 + cityTypes2) * thresholdScalar)) {
		return 2;
	}

	return -1;
}

void CVG_CreatureManager::NewCityAppeared() {
	if (!IsCivGame()) { return; }
	//SporeDebugPrint("New City Appeared.");

	auto cities = GetData<cCity>();
	auto city = cities[cities.size()-1];
	bool isplayercity = city->IsPlayerOwned();

	// NOTE: normally, player cities will NEVER spawn on their own.
	// This is mostly used for when the first player city spawns.
	if (isplayercity) {
		MessageManager.MessageSend(id("PlayerCitySpawned"),nullptr);
	}
	else {
		// return if not enough cities for city type distribution to matter
		if (cities.size() < 5 || isplayercity) {
			return;
		}

		int citytype = DetermineNeededCityType();

		if (citytype != -1 && citytype != city->mVehicleSpecialty) {
			//SporeDebugPrint("Changing city %i from type %i to type %i to ensure balance.", cities.size(), city->mVehicleSpecialty, citytype);
			city->mVehicleSpecialty = citytype;
		}
	}
	
}

uint32_t CVG_CreatureManager::GetReactionAnimation() const {
	return civReactionAnims[rand(civReactionAnims.size())];
}

void CVG_CreatureManager::CitizenPlayReaction(cCreatureCitizenPtr citizen) {
	citizen->PlayAnimation(GetReactionAnimation());
}


bool CVG_CreatureManager::HandleMessage(uint32_t messageID, void* msg) {
	if (messageID == id("NewCityAppeared")) {
		NewCityAppeared();
	}
	return false;
}

int CVG_CreatureManager::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

// The method that receives the message.
bool CVG_CreatureManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (message.Mouse.mouseButton == kMouseButtonLeft) {
		if (message.IsType(kMsgMouseDown)) {
			mbMouseClicked = true;
		}
		else if (message.IsType(kMsgMouseMove)) {
			mbMouseClicked = false;
		}
		else if (message.IsType(kMsgMouseUp)) {
			if (mbMouseClicked) {
				auto citizen = GetHoveredCitizen();
				if (citizen) {
					// cancel animation and play new one
					citizen->Teleport(citizen->GetPosition(), citizen->GetOrientation());
					CitizenPlayReaction(citizen);
				}
				
			}
			mbMouseClicked = false;
		}
	}
	

	// Return true if the message was handled
	return false;
}