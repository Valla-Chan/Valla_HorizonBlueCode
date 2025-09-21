#pragma once

#include <Spore\BasicIncludes.h>

//#define CapabilityChecker  (*cCapabilityChecker::Get())
//#define cCapabilityCheckerPtr intrusive_ptr<cCapabilityChecker>

namespace CapabilityChecker 
{
	const uint32_t TYPE = id("cCapabilityChecker");
	enum Symmetry {
		LEFT = 0x0F48EB09,
		RIGHT = 0x18C1DBE0,
		CENTER = 0x3A3B9D21
	};

	int GetCapabilityLevel(const cCreatureBasePtr& creature, const uint32_t propertyID);
	bool IsPartOnCreature(const cCreatureBasePtr& creature, const ResourceKey part);
	int GetPartUnlockLevel(const ResourceKey part);
	int GetCapabilityLevelFromResource(const ResourceKey speciesKey, const uint32_t propertyID);
	vector<Editors::cEditorResourceBlock> GetCreationRigblocks(const ResourceKey speciesKey);
	vector<ResourceKey> GetCreationRigblockKeys(const ResourceKey speciesKey);
	Vector3 GetPosFromPartCapability(const cCreatureBasePtr& creature, const uint32_t propertyID);
	//
	bool HasModelKeyValue(const ResourceKey& modelKey, const uint32_t property);
	ResourceKey GetModelKeyValue(const ResourceKey& modelKey, const uint32_t property);
	vector<ResourceKey> GetModelKeyValues(const ResourceKey& modelKey, const uint32_t property);
	ResourceKey GetModelParentKey(const ResourceKey& modelKey);
	//bool IsModelSymmetricVariant(const ResourceKey& modelKey);
	ResourceKey GetSymmetricVariant(const ResourceKey& modelKey, const Symmetry symmetry = Symmetry::LEFT, bool fallback = true);
	//
	LocalizedString GetModelText(const ResourceKey& modelKey, const uint32_t property);
	const char16_t* GetModelString16(const ResourceKey& modelKey, const uint32_t property);
	LocalizedString GetModelBlockName(const ResourceKey& modelKey);
	//
	bool HasModelUInt32Value(const ResourceKey& modelKey, const uint32_t property);
	uint32_t GetModelUInt32Value(const ResourceKey& modelKey, const uint32_t property) ;
	uint32_t GetModelKeyInstanceID(const ResourceKey& modelKey, const uint32_t property);
	//
	float GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property);
	int GetModelIntValue(const ResourceKey& modelKey, const uint32_t property);
	vector<int> GetModelIntValues(const ResourceKey& modelKey, const uint32_t property);
	bool GetModelBoolValue(const ResourceKey& modelKey, const uint32_t property);
	Vector2* GetModelVector2sValue(const ResourceKey& modelKey, const uint32_t property);
	ColorRGB GetModelColorRGBValue(const ResourceKey& modelKey, const uint32_t property);
	//

};
