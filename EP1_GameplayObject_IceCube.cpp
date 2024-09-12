#include "stdafx.h"
#include "EP1_GameplayObject_IceCube.h"
#include "CapabilityChecker.h"

EP1_GameplayObject_IceCube::EP1_GameplayObject_IceCube()
{
	App::AddUpdateFunction(this);
}


EP1_GameplayObject_IceCube::~EP1_GameplayObject_IceCube()
{
}


void EP1_GameplayObject_IceCube::Update()
{

	for (auto creature : mFrozenCreatures) {
		SetCreatureFrozen(creature, true);
	}
}

//------------------------------------------------------------------------------------------------

bool EP1_GameplayObject_IceCube::IsHandledObject(cSpatialObjectPtr object) const {
	return CapabilityChecker.GetModelBoolValue(object->GetModelKey(), id("modelCapabilityIce")) == true;
}
 
//------------------------------------------------------------------------------------------------

// get the closest creature to a gameplay object
cCreatureAnimalPtr EP1_GameplayObject_IceCube::GetClosestCreature(cSpatialObjectPtr gameplayob) {
	auto origin = gameplayob->GetPosition();
	float last_dist = gameplayob->GetScale() * 4.0f;
	cCreatureAnimalPtr found_creature = nullptr;
	auto avatar = GameNounManager.GetAvatar();

	auto creatures = Simulator::GetDataByCast<Simulator::cCreatureAnimal>();
	for (auto creature : creatures) {
		float dist = Math::distance(origin, creature->GetPosition());
		if (dist < last_dist && creature != avatar) {
			found_creature = creature;
			last_dist = dist;
		}
	}
	return found_creature;
}



void EP1_GameplayObject_IceCube::BreakIce(cSpatialObjectPtr object) {
	
}

// get the objects that freeze the creatures
eastl::vector<cSpatialObjectPtr> EP1_GameplayObject_IceCube::GetIceCubes() const {
	eastl::vector<cSpatialObjectPtr> icecubes = {};

	auto objects = Simulator::GetDataByCast<Simulator::cSpatialObject>();
	for (auto object : objects) {
		auto powerup = object_cast<cScenarioPowerup>(object);
		if (powerup && IsObjectIce(object)) {
		//if (object->GetModelKey().instanceID == 0x93512140) { //temp crate test
			icecubes.push_back(object);
		}
	}
	return icecubes;
}


// for each ice block, apply the nearest creatures 
void EP1_GameplayObject_IceCube::ApplyFrozenToIce() {
	mFrozenCreatures.clear();
	mFrozenCreaturePositions.clear();
	// loop through the ice cubes
	for (auto ice : GetIceCubes()) {
		auto creature = GetClosestCreature(ice);
		auto iceOrnament = object_cast<cCombatant>(ice);
		if (iceOrnament) {
			iceOrnament->mMaxHealthPoints = 100.0f;
			iceOrnament->mHealthPoints = 50.0f;
		}
		if (creature) {
			mFrozenCreatures.push_back(creature);
			mFrozenCreaturePositions.push_back(creature->GetPosition());
			SetCreatureFrozen(creature,true);
		}
	}
}

const auto frozenColor = ColorRGB(2.4f, 2.5f, 2.6f);
void EP1_GameplayObject_IceCube::SetCreatureFrozen(cCreatureAnimalPtr creature, bool frozen) {
	if (creature && creature->GetModel()) {
		if (frozen) {
			creature->mbDead = true;
			creature->mbEnabled = false;
			creature->mbFixed = true;
			creature->SetIdentityColor(frozenColor);
			creature->PlayAnimation(0xD9D38CD2); // "ep1_icecube_freeze"
			for (size_t i = 0; 1 < mFrozenCreatures.size(); i++) {
				if (mFrozenCreatures[i] == creature) {
					creature->Teleport(mFrozenCreaturePositions[i], creature->GetOrientation());
				}
			}
			
		}
		else {
			if (creature->GetIdentityColor() == frozenColor) {
				creature->mbDead = false;
				creature->mbEnabled = true;
				creature->mbFixed = false;
				creature->SetIdentityColor(ColorRGB(1.0f, 1.0f, 1.0f));
				creature->PlayAnimation(0xE70F735E); // "CRG_epicstomp_react"
			}
		}
	}
}


bool EP1_GameplayObject_IceCube::HandleMessage(uint32_t messageID, void* msg)
{
	if (messageID == kMsgScenarioRedo || messageID == kMsgScenarioUndo) {
		if (IsScenarioMode()) {
			ApplyFrozenToIce();
		}
	}
	else if (messageID == kMsgSwitchGameMode) {
		auto task = Simulator::ScheduleTask(this, &EP1_GameplayObject_IceCube::FireIfScenario, 0.2f);
	}
	return false;
}

void EP1_GameplayObject_IceCube::FireIfScenario() {
	if (IsScenarioMode()) {
		ApplyFrozenToIce();
	}
}


//------------------------------------------------------------------------------------------------


// For internal use, do not modify.
int EP1_GameplayObject_IceCube::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EP1_GameplayObject_IceCube::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_GameplayObject_IceCube::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(EP1_GameplayObject_IceCube);
	return nullptr;
}
