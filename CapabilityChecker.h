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
	bool IsPartOnCreature(const cCreatureBasePtr& creature, const ResourceKey part) const;
	int GetPartUnlockLevel(const ResourceKey part) const;
	int GetCapabilityLevelFromResource(const ResourceKey speciesKey, const uint32_t propertyID) const;
	Vector3 GetPosFromPartCapability(const cCreatureBasePtr& creature, const uint32_t propertyID) const;
	//
	bool HasModelKeyValue(const ResourceKey& modelKey, const uint32_t property) const;
	ResourceKey GetModelKeyValue(const ResourceKey& modelKey, const uint32_t property) const;
	ResourceKey GetModelParentKey(const ResourceKey& modelKey) const;
	//
	LocalizedString GetModelText(const ResourceKey& modelKey, const uint32_t property) const;
	eastl::string16& GetModelString16(const ResourceKey& modelKey, const uint32_t property) const;
	LocalizedString GetModelBlockName(const ResourceKey& modelKey) const;
	//
	bool HasModelUInt32Value(const ResourceKey& modelKey, const uint32_t property) const;
	uint32_t GetModelUInt32Value(const ResourceKey& modelKey, const uint32_t property) const;
	uint32_t GetModelKeyInstanceID(const ResourceKey& modelKey, const uint32_t property) const;
	//
	float GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property) const;
	int GetModelIntValue(const ResourceKey& modelKey, const uint32_t property) const;
	bool GetModelBoolValue(const ResourceKey& modelKey, const uint32_t property) const;
	Vector2* GetModelVector2sValue(const ResourceKey& modelKey, const uint32_t property) const;
	ColorRGB GetModelColorRGBValue(const ResourceKey& modelKey, const uint32_t property) const;
	//


	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

protected:
	static cCapabilityChecker* sInstance;
};
