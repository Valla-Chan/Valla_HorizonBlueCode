#pragma once

#include <Spore\BasicIncludes.h>

#define CRG_DiseaseManagerPtr intrusive_ptr<CRG_DiseaseManager>

using namespace Simulator;

class CRG_DiseaseManager 
	: public Object
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("CRG_DiseaseManager");
	
	CRG_DiseaseManager();
	~CRG_DiseaseManager();
	void Update() override;

	void ApplyNestSicknessChance(cNestPtr nest);

	eastl::vector<cCreatureBasePtr> GetDiseasedCreatures() const;
	void CRG_DiseaseManager::ApplyDiseaseAmount(eastl::vector<cCreatureBasePtr> diseased);

	void CRG_DiseaseManager::DrainStats(cCreatureBasePtr creature);
	void CRG_DiseaseManager::MakeCreatureSick(cCreatureBasePtr creature);
	void CRG_DiseaseManager::CureCreature(cCreatureBasePtr creature);

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
