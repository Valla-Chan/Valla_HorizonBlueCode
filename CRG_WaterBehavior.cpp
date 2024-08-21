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

		//App::ConsolePrintF("X:  %f, Y:  %f,  Z: %f", avatar->GetOrientation().ToEuler().x, avatar->GetOrientation().ToEuler().y, avatar->GetOrientation().ToEuler().z);
		

		bool in_water = PlanetModel.IsInWater(avatar->GetPosition());
		if (in_water && !avatar->mbSupported && avatar->mHealthPoints >= waterdamage) {

			/*
			if (Simulator::cPlanetModel::Get()->mpTerrain2) {
				App::ConsolePrintF("mpTerrain2 is valid!");
			}
			else {
				App::ConsolePrintF("mpTerrain2 is INVALID...");
			}*/
			//avatar->mbKeepPinnedToPlanet = true;
			//avatar->SetPosition(Simulator::cPlanetModel::Get()->ToSurface(avatar->GetPosition()));
			//Vector3 pos = PlanetModel.ToSurface(avatar->GetPosition());
			
			//avatar->SetHealthPoints(avatar->mHealthPoints - waterdamage);
			//Math::Quaternion orientation = Simulator::cPlanetModel::Get()->GetOrientation(avatar->GetPosition(), Vector3(90, 0, 0));
			//avatar->SetVelocity( (orientation.ToEuler().Normalized() * 2.0f) + (avatar->GetOrientation().ToEuler().Normalized() * 2.0f)); //avatar->GetVelocity() +
			
		}

		//caplvl_swim = CapabilityChecker.GetCapabilityLevel(avatar, 0x073CE5DD);
		
		//Math::Quaternion orientation = Simulator::cPlanetModel::Get()->GetOrientation(avatar->GetPosition(), Vector3(-90, 0, 0));
		
		
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
