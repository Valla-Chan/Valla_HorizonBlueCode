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

// TODO: turn this into a standalone class
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
