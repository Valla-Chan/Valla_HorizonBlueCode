#pragma once

#include <Spore\BasicIncludes.h>

#define CapabilityChecker  (*cCapabilityChecker::Get())
#define cCapabilityCheckerPtr intrusive_ptr<cCapabilityChecker>

class cCapabilityChecker 
	: public Object
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("cCapabilityChecker");
	
	cCapabilityChecker();
	~cCapabilityChecker();

	static cCapabilityChecker* Get();

	int GetCapabilityLevel(const cCreatureBasePtr& creature, const uint32_t propertyID) const;
	ResourceKey cCapabilityChecker::GetModelKeyValue(const ResourceKey& modelKey, const uint32_t property) const;
	float cCapabilityChecker::GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property) const;
	int cCapabilityChecker::GetModelIntValue(const ResourceKey& modelKey, const uint32_t property) const;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

protected:
	static cCapabilityChecker* sInstance;
};
