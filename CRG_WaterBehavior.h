#pragma once

#include <Spore\BasicIncludes.h>

#define CRG_WaterBehaviorPtr intrusive_ptr<CRG_WaterBehavior>

class CRG_WaterBehavior 
	: public Object
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("CRG_WaterBehavior");
	
	CRG_WaterBehavior();
	~CRG_WaterBehavior();

	void Update() override;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
