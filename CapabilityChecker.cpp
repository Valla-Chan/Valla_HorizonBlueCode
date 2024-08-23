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

// Open a model resource and find a float value of a property
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

// Open a model resource and find an int value of a property
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
