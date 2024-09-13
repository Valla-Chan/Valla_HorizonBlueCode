#include "stdafx.h"
#include "EP1_GameplayObject_IceCube.h"
#include "CapabilityChecker.h"

EP1_GameplayObject_IceCube::EP1_GameplayObject_IceCube()
{
}


EP1_GameplayObject_IceCube::~EP1_GameplayObject_IceCube()
{
}


void EP1_GameplayObject_IceCube::Update()
{
	for (auto item : mpIngameObjects) {
		SetCreatureFrozen(item.activator, true);
	}
}

//------------------------------------------------------------------------------------------------

bool EP1_GameplayObject_IceCube::IsHandledObject(cSpatialObjectPtr object) const {
	if (object) {
		return CapabilityChecker.GetModelBoolValue(object->GetModelKey(), id("modelCapabilityIce")) == true;
	}
	return false;
}

void EP1_GameplayObject_IceCube::ApplyCombatantEffect(cCombatantPtr combatant, cSpatialObjectPtr object) {
	auto creature = object_cast<cCreatureAnimal>(combatant);
	if (creature) {
		SetCreatureFrozen(creature, true);
		/*
		// commented out because this probably messes up movement after the ice breaks.
		if (IsPlayingAdventure()) {
			creature->mStandardSpeed = 0.0f;
			creature->mTurnRate = 0.0f;
		}*/
	}
}

void EP1_GameplayObject_IceCube::ResetCombatantEffect(cCombatantPtr combatant) {
	auto creature = object_cast<cCreatureAnimal>(combatant);
	SetCreatureFrozen(creature, false);
}

void EP1_GameplayObject_IceCube::OnDamaged(cCombatantPtr object, float damage, cCombatantPtr pAttacker) {
	BreakIce(object_cast<cSpatialObject>(object));
}

//------------------------------------------------------------------------------------------------

void EP1_GameplayObject_IceCube::BreakIce(cSpatialObjectPtr object) {
	auto combatant = object_cast<cCreatureAnimal>(GetIngameObject(object).activator);
	if (combatant) {
		SetCreatureFrozen(combatant, false);
	}
	// debug test
	object->SetScale(0.01);
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

			// store position to hold
			//mFrozenCreaturePositions.push_back(creature->GetPosition());
			//for (size_t i = 0; 1 < mFrozenCreatures.size(); i++) {
			//	if (mFrozenCreatures[i] == creature) {
			//		creature->Teleport(mFrozenCreaturePositions[i], creature->GetOrientation());
			//	}
			//}
			
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
