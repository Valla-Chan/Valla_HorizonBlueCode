#include "stdafx.h"
#include "CRG_DiseaseManager.h"
#include <Spore\Simulator.h>

CRG_DiseaseManager::CRG_DiseaseManager()
{
	App::AddUpdateFunction(this);
}


CRG_DiseaseManager::~CRG_DiseaseManager()
{
}

void CRG_DiseaseManager::Update()
{
	ApplyDiseaseAmount(GetDiseasedCreatures());
}

void CRG_DiseaseManager::ApplyNestSicknessChance(cNestPtr nest) {

}

eastl::vector<cCreatureBasePtr> CRG_DiseaseManager::GetDiseasedCreatures() const
{
	eastl::vector<cCreatureBasePtr> diseased;
	auto creatures = Simulator::GetDataByCast<Simulator::cCreatureBase>();
	for (auto creature : creatures) {
		if (creature->mbIsDiseased) {
			diseased.push_back(creature);
		}
	}
	return diseased;
}

const float colordrop = 0.0001f;
void CRG_DiseaseManager::ApplyDiseaseAmount(eastl::vector<cCreatureBasePtr> diseased)
{
	if (!(IsCreatureGame() || IsTribeGame())) { return; }
	for (auto creature : diseased) {
		
		if (creature->mbIsDiseased) {
			// do not allow babies to get sick, cure avatar when they die
			if (creature->mAge < 1 || (object_cast<Simulator::cCreatureBase>(GameNounManager.GetAvatar()) == creature && creature->mbDead) ) {
				CureCreature(creature);
				return;
			}

			// if creature is diseased and above the color white, drop the color.
			if (creature->GetIdentityColor().r >= 1.105f) {
				ColorRGB newcolor = creature->GetIdentityColor();
				// if dead, stay sick.
				if (!creature->mbDead) {
					// slow drain
					if (creature->GetIdentityColor().r > 1.25f) {
						newcolor.Set(newcolor.r - colordrop, newcolor.g - colordrop, newcolor.b);
						DrainStats(creature);
					}
					// faster drain near the end
					else {
						newcolor.Set(newcolor.r - colordrop*10, newcolor.g - colordrop * 10, newcolor.b);
					}
				}
				creature->SetIdentityColor(newcolor);
			}
			// if creature is diseased and color has dropped enough, set it to not diseased anymore.
			else if (creature->GetIdentityColor().r < 1.105f && creature->GetIdentityColor().r > 1.1f && creature->GetIdentityColor() != ColorRGB(1,1,1)) {
				CureCreature(creature);
			}
			// if creature is diseased and not yet colored, color it.
			else {
				MakeCreatureSick(creature);
			}
		}
		
	}
}

// drain health and hunger
void CRG_DiseaseManager::DrainStats(cCreatureBasePtr creature) {
	if (creature->mHealthPoints > 1.0f) {
		creature->SetHealthPoints(creature->mHealthPoints - 0.01f);
	}
	if (creature->mHunger > 5.0f) {
		creature->mHunger -= 0.02f;
	}
	creature->mbStealthed = false;
}

// add the disease effect
void CRG_DiseaseManager::MakeCreatureSick(cCreatureBasePtr creature) {
	creature->mbIsDiseased = true;
	creature->SetIdentityColor(ColorRGB(1.6f, 1.6f, 0.7f));
	if (!creature->GetEffectFromPools(id("crg_disease"))) {
		creature->CreateAndStartEffectForPool1(id("crg_disease"));
	}
}

// remove the disease effect
void CRG_DiseaseManager::CureCreature(cCreatureBasePtr creature) {
	if (creature->mbIsDiseased) {

		creature->mbIsDiseased = false;
		creature->SetIdentityColor(ColorRGB(1, 1, 1));
		creature->StopEffectFromPools(id("crg_disease"));
		if (creature->mHealthPoints < creature->mMaxHealthPoints) {
			creature->SetHealthPoints(creature->mHealthPoints + 1.0f);
		}
		if (creature->mHunger < 100.0f) {
			creature->mHunger += 5.0f;
		}
		if (!creature->mbDead) {
			// sneeze
			creature->PlayAnimation(0xE38512BD);
		}
	}
}


// For internal use, do not modify.
int CRG_DiseaseManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CRG_DiseaseManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* CRG_DiseaseManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(CRG_DiseaseManager);
	return nullptr;
}
