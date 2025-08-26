#pragma once

#include <Spore\BasicIncludes.h>

#define ArenaCreaturePtr intrusive_ptr<ArenaCreature>

class ArenaCreature 
	: public Object
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("ArenaCreature");
	static const uint32_t DEATHMSG = id("ArenaCreatureDied"); // when a creature dies, this msg is emitted with the ArenaCreature as the data
	
	ArenaCreature(const IAnimWorldPtr animworld, const ResourceKey& crtkey, Vector3 position = Vector3(), Quaternion orientation = Quaternion());
	~ArenaCreature();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

	//-------------------------------------
	// Methods
	
	// Stats
	const AnimatedCreaturePtr GetCreature() const;
	void SetHealth(float health);
	float GetHealth() const;
	void TakeDamage(float dmg);
	void RestoreHealth(float health);

	// Movement
	void MoveTo(Vector3 position);

	//-------------------------------------
	// Vars
	AnimatedCreaturePtr mpCreature;


protected:

	bool mbPlayer = false;

	float mMaxHealth = 100.0f;
	float mHealth = mMaxHealth;

	int mCurrentAbility = -1;

	float mMoveSpeed = 5.0f;

private:

	void Die();

};
