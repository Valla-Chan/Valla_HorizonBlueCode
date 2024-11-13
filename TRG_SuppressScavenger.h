#pragma once

#include <Spore\BasicIncludes.h>

#define TRG_SuppressScavengerPtr intrusive_ptr<TRG_SuppressScavenger>

using namespace Simulator;
class TRG_SuppressScavenger 
	: public Object
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("TRG_SuppressScavenger");
	
	TRG_SuppressScavenger();
	~TRG_SuppressScavenger();

	bool mbSuppressed = false;
	void SetSuppressed(bool state);
	bool SuppressScavenger();
	void RemoveScavengerCreature();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
