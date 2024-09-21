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
	auto item = GetIngameObject(object);
	auto creature = object_cast<cCreatureAnimal>(item.activator);
	if (creature) {
		SetCreatureFrozen(creature, false);
		auto effect1 = creature->CreateAndStartEffectForPool1(id("EP1_shaman_freeze_react_b"));
		auto effect2 = creature->CreateAndStartEffectForPool1(0x7BB9D4A5); // poof white

		auto effectscale = effect1->GetSourceTransform().GetScale();
		effectscale = effectscale * creature->GetScale();

		effect1->SetSourceTransform(effect1->GetSourceTransform().SetScale(effectscale));
		effect2->SetSourceTransform(effect2->GetSourceTransform().SetScale(effectscale));
	}
	Destroy(object);
}

void EP1_GameplayObject_IceCube::SetCreatureFrozen(cCreatureAnimalPtr creature, bool frozen) {
	if (creature && creature->GetModel()) {
		if (frozen) {
			creature->mbDead = true;
			creature->mbEnabled = false;
			creature->mbFixed = true;
			creature->SetIdentityColor(frozenColor);
			creature->PlayAnimation(0xD9D38CD2); // "ep1_icecube_freeze"
		}
		else if (!creature->mbEnabled) {
			creature->mbDead = false;
			creature->mbEnabled = true;
			creature->mbFixed = false;
			creature->SetIdentityColor(ColorRGB(1.0f, 1.0f, 1.0f));
			creature->PlayAnimation(0xE70F735E); // "CRG_epicstomp_react"
			//creature->PlayAnimation(0x07AD2BF7); // "EP1_react_freeze_CrTr_alive"
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
