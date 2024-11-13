#include "stdafx.h"
#include "TRG_SuppressScavenger.h"

TRG_SuppressScavenger::TRG_SuppressScavenger()
{
}


TRG_SuppressScavenger::~TRG_SuppressScavenger()
{
}

//------------------------------------------------------

// TODO: make the scarecrow call this.
void TRG_SuppressScavenger::SetSuppressed(bool state) {
	mbSuppressed = state;
}


bool TRG_SuppressScavenger::SuppressScavenger() {
	if (IsTribeGame()) {
		if (mbSuppressed) {
			Simulator::ScheduleTask(this, &TRG_SuppressScavenger::RemoveScavengerCreature, 1.0f);
		}
	}
	return mbSuppressed;
	
}

void TRG_SuppressScavenger::RemoveScavengerCreature() {
	if (IsTribeGame()) {
		for (auto creature : GetData<cCreatureAnimal>()) {
			// If creature is the scavenger type
			if (creature && creature->mHerd && creature->mHerd->mArchetype == id("TRG_Food_Scavenger")) {
				// Send him to hell
				creature->Teleport(Vector3(0, 0, 0), creature->mOrientation);
				creature->SetHealthPoints(0);
			}
		}
	}
	
}







//------------------------------------------------------

// For internal use, do not modify.
int TRG_SuppressScavenger::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int TRG_SuppressScavenger::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* TRG_SuppressScavenger::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(TRG_SuppressScavenger);
	return nullptr;
}
