#include "stdafx.h"
#include "CRG_WaterBehavior.h"
#include "CapabilityChecker.h"

CRG_WaterBehavior::CRG_WaterBehavior()
{
	App::AddUpdateFunction(this);
}


CRG_WaterBehavior::~CRG_WaterBehavior()
{
}

//const uint32_t animDrown = 0xB3A26EF1; //"gen_swim_drown"
const uint32_t animDrown = 0x04286319; //"sp_death_water"
const float waterdamage = 0.01f;
int caplvl_swim = 0;

void CRG_WaterBehavior::Update()
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();

	if (avatar && !GameTimeManager.IsPaused()) {

		//caplvl_swim = CapabilityChecker::GetCapabilityLevel(avatar, 0x073CE5DD);

		// TODO: detect if the position is below water level. Right now this will return true even if far above the water.
		bool in_water = PlanetModel.IsInWater(avatar->GetPosition());
		if (in_water && avatar->mHealthPoints > 0) {

			// Swim 0: drown
			if (caplvl_swim < 1) {
				// Drain health
				avatar->SetHealthPoints(max(avatar->mHealthPoints - waterdamage, 0.0f));

				//// SHOVE THE ACTOR DOWNWARDS
				// Get the avatar's up and forward vectors
				Vector3 dirUp = avatar->GetPosition().Normalized();
				Vector3 dirFwd = avatar->GetDirection();

				// Get the avatar's current velocity vector
				Vector3 velocity = avatar->GetVelocity();

				// Calculate the speed only in the up direction
				float speedInDirection = avatar->GetVelocity().Dot(dirUp);

				Vector3 velocityAdditive = (dirUp * (-speedInDirection - 5.0f));

				avatar->SetVelocity(velocity + velocityAdditive);

				// TODO: fix this
				/*
				if (avatar->mHealthPoints < avatar->mMaxHealthPoints / 2.0f) {
					uint32_t* anim = nullptr;
					avatar->mpAnimatedCreature->GetCurrentAnimation(anim);
					if (anim && anim[0] != animDrown) {
						avatar->PlayAnimation(animDrown);
					}
				}
				*/
				
			}
			
		}

		
		
		
	}
}

// For internal use, do not modify.
int CRG_WaterBehavior::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CRG_WaterBehavior::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* CRG_WaterBehavior::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(CRG_WaterBehavior);
	return nullptr;
}
