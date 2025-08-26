#include "stdafx.h"
#include "ArenaCreature.h"

ArenaCreature::ArenaCreature(const IAnimWorldPtr animworld, const ResourceKey& crtkey, Vector3 position, Quaternion orientation)
{
	mpCreature = animworld->LoadCreature(crtkey);
	if (mpCreature) {
		mpCreature->mPosition = position;
		mpCreature->mOrientation = orientation;
	}
}


ArenaCreature::~ArenaCreature()
{
}

// For internal use, do not modify.
int ArenaCreature::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ArenaCreature::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* ArenaCreature::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(ArenaCreature);
	return nullptr;
}

//--------------------------------------------------------------------------------

const AnimatedCreaturePtr ArenaCreature::GetCreature() const {
	return mpCreature;
}

void ArenaCreature::SetHealth(float health) {
	mHealth = clamp(health, 0.0f, mMaxHealth);
}

float ArenaCreature::GetHealth() const {
	return mHealth;
}

void ArenaCreature::TakeDamage(float dmg) {
	SetHealth(clamp(mHealth - dmg, 0.0f, mMaxHealth));

	// TODO: account for different dmg types, anims, etc

	if (mHealth == 0) {
		Die();
	}
}

void ArenaCreature::RestoreHealth(float health) {
	SetHealth(clamp(mHealth + health, 0.0f, mMaxHealth));
}

void ArenaCreature::Die() {
	MessageManager.MessageSend(DEATHMSG, this);
}


// Movement

void ArenaCreature::MoveTo(Vector3 position) {
	//mpCreature->
}