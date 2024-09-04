#include "stdafx.h"
#include "CapabilityChecker.h"

cCapabilityChecker::cCapabilityChecker()
{
	sInstance = this;
}

cCapabilityChecker::~cCapabilityChecker()
{
	sInstance = nullptr;
}

cCapabilityChecker* cCapabilityChecker::sInstance;

cCapabilityChecker* cCapabilityChecker::Get()
{
	return sInstance;
}

int cCapabilityChecker::GetCapabilityLevel(const cCreatureBasePtr& creature, const uint32_t propertyID) const
{
	int32_t caplvl = 0;
	if (creature) {
		ResourceObjectPtr res;
		if (ResourceManager.GetResource(creature->mSpeciesKey, &res))
		{
			auto resource = object_cast<Editors::cEditorResource>(res);
			// Loop through the creature parts to find this capability
			for (size_t i = 0; i < resource->mBlocks.size(); i++) {
				Editors::cEditorResourceBlock block = resource->mBlocks[i];
				PropertyListPtr mpPropList;
				if (PropManager.GetPropertyList(block.instanceID, block.groupID, mpPropList))
				{
					int cap_dst;
					App::Property::GetInt32(mpPropList.get(), propertyID, cap_dst);
					if (cap_dst > caplvl) {
						caplvl = cap_dst;
					}
				}
			}

		}
	}
	return caplvl;
}

// Open a model resource and find if a property key exists in the file
bool cCapabilityChecker::HasModelKeyValue(const ResourceKey& modelKey, const uint32_t property) const
{
	PropertyListPtr mpPropList;
	ResourceKey key;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		return App::Property::GetKey(mpPropList.get(), property, key);
	}
	else { return false; }
}

// Open a model resource and find a key from a property
ResourceKey cCapabilityChecker::GetModelKeyValue(const ResourceKey& modelKey, const uint32_t property) const
{
	PropertyListPtr mpPropList;
	ResourceKey key;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		bool test = App::Property::GetKey(mpPropList.get(), property, key);
		return key;
	}
	else { return ResourceKey(0, 0, 0); }
}

// Open a model resource and find if a property uint32 exists in the file
bool cCapabilityChecker::HasModelUInt32Value(const ResourceKey& modelKey, const uint32_t property) const
{
	PropertyListPtr mpPropList;
	uint32_t value;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		return App::Property::GetUInt32(mpPropList.get(), property, value);
	}
	else { return false; }
}

// Open a model resource and get a uint32 value of a property
uint32_t cCapabilityChecker::GetModelUInt32Value(const ResourceKey& modelKey, const uint32_t property) const
{
	PropertyListPtr mpPropList;
	uint32_t value;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		bool test = App::Property::GetUInt32(mpPropList.get(), property, value);
		if (test) {
			return value;
		}
	}
	return 0x0;
}

// Open a model resource and get a float value of a property
float cCapabilityChecker::GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property) const
{
	PropertyListPtr mpPropList;
	float value;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		bool test = App::Property::GetFloat(mpPropList.get(), property, value);
		if (test) {
			return value;
		}
	}
	return 0.0f;
}

// Open a model resource and get an array of vector2 values of a property
Vector2* cCapabilityChecker::GetModelVector2sValue(const ResourceKey& modelKey, const uint32_t property) const
{
	//Initialize some pointers
	Vector2* partLevels;
	size_t numParts;

	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		bool test = App::Property::GetArrayVector2(mpPropList.get(), property, numParts, partLevels);
		if (test) {
			return partLevels;
		}
	}
	return nullptr;
}

// Open a model resource and get an int value of a property
int cCapabilityChecker::GetModelIntValue(const ResourceKey& modelKey, const uint32_t property) const
{
	PropertyListPtr mpPropList;
	int value;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		bool test = App::Property::GetInt32(mpPropList.get(), property, value);
		if (test) {
			return value;
		}
	}
	return 0;
}

// Open a model resource and get a bool value of a property
bool cCapabilityChecker::GetModelBoolValue(const ResourceKey& modelKey, const uint32_t property) const
{
	PropertyListPtr mpPropList;
	bool value;
	if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
	{
		bool test = App::Property::GetBool(mpPropList.get(), property, value);
		if (test) {
			return value;
		}
	}
	return false;
}


// For internal use, do not modify.
int cCapabilityChecker::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cCapabilityChecker::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cCapabilityChecker::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(cCapabilityChecker);
	return nullptr;
}
