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

const float waterdamage = 0.01f;
int caplvl_swim = 0;

void CRG_WaterBehavior::Update()
{
	cCreatureAnimalPtr avatar = GameNounManager.GetAvatar();

	if (avatar) {
		bool in_water = Simulator::cPlanetModel::Get()->IsInWater(avatar->GetPosition());
		if (in_water && avatar->mHealthPoints >= waterdamage) {
			avatar->SetHealthPoints(avatar->mHealthPoints - waterdamage);
		}

		int caplvl = CapabilityChecker.GetCapabilityLevel(avatar, 0x073CE5DD);
		//App::ConsolePrintF("Avatar creature has %i energy recharge.", caplvl);
		if (caplvl > 0) {
			//App::ConsolePrintF("Avatar creature has energy recharge.");
			avatar->mEnergy += 0.5 * GameTimeManager.GetSpeed();
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
