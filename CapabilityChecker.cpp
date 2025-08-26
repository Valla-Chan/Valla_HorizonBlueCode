#include "stdafx.h"
#include "CapabilityChecker.h"

namespace CapabilityChecker
{

	int GetCapabilityLevel(const cCreatureBasePtr& creature, const uint32_t propertyID)
	{
		if (creature) {
			return GetCapabilityLevelFromResource(creature->mSpeciesKey, propertyID);
		}
		return 0;
	}


	bool IsPartOnCreature(const cCreatureBasePtr& creature, const ResourceKey part)
	{
		if (creature) {

			// holds left, right, and center keys for part.
			vector<ResourceKey> partKeys;
			partKeys.push_back(part);

			{
				ResourceKey part_right = GetModelKeyValue(part, 0x18C1DBE0);
				ResourceKey part_left = GetModelKeyValue(part, 0x0F48EB09);
				ResourceKey part_centered = GetModelKeyValue(part, 0x3A3B9D21);

				if (part_right.instanceID != 0x0 && part_right.instanceID != part.instanceID) {
					partKeys.push_back(part_right);
				}
				if (part_left.instanceID != 0x0 && part_left.instanceID != part.instanceID) {
					partKeys.push_back(part_left);
				}
				if (part_centered.instanceID != 0x0 && part_centered.instanceID != part.instanceID) {
					partKeys.push_back(part_centered);
				}

			}

			ResourceObjectPtr res;
			if (ResourceManager.GetResource(creature->mSpeciesKey, &res)) {
				auto resource = object_cast<Editors::cEditorResource>(res);

				// Loop through the creature parts to find the desired ones.
				for (size_t i = 0; i < resource->mBlocks.size(); i++) {
					Editors::cEditorResourceBlock block = resource->mBlocks[i];
					for (size_t j = 0; j < partKeys.size(); j++) {
						if (block.instanceID == partKeys[j].instanceID) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	int GetPartUnlockLevel(const ResourceKey part)
	{
		int level = GetModelIntValue(part, 0x03A289AC);
		if (level > 1) { return level; }
		return 1;
	}

	int GetCapabilityLevelFromResource(const ResourceKey speciesKey, const uint32_t propertyID)
	{
		int32_t caplvl = 0;

		ResourceObjectPtr res;
		if (ResourceManager.GetResource(speciesKey, &res)) {
			auto resource = object_cast<Editors::cEditorResource>(res);
			// Loop through the creature parts to find the highest capability
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
		return caplvl;
	}

	// Returns local pos to part. will need to be added to creature pos to make global.
	// return (0,0,0) if not found
	Vector3 GetPosFromPartCapability(const cCreatureBasePtr& creature, const uint32_t propertyID)
	{
		int32_t caplvl = 0;
		int partidx = -1;
		Vector3 partpos = Vector3(0, 0, 0);
		if (creature) {
			ResourceObjectPtr res;
			if (ResourceManager.GetResource(creature->mSpeciesKey, &res))
			{
				auto resource = object_cast<Editors::cEditorResource>(res);
				// Loop through the creature parts to find this capability at its highest level
				for (size_t i = 0; i < resource->mBlocks.size(); i++) {
					Editors::cEditorResourceBlock block = resource->mBlocks[i];
					PropertyListPtr mpPropList;
					if (PropManager.GetPropertyList(block.instanceID, block.groupID, mpPropList))
					{
						int cap_dst;
						App::Property::GetInt32(mpPropList.get(), propertyID, cap_dst);
						if (cap_dst > caplvl) {
							caplvl = cap_dst;
							partidx = i;
						}
					}
				}
				if (partidx > -1) {
					// get part with highest capability
					auto part = resource->mBlocks[partidx];
					// return pos modified to account for creature rotation
					auto localoffset = Vector3(-part.position.x, -part.position.y, part.position.z);
					partpos = creature->GetOrientation().ToMatrix().Transposed() * localoffset * creature->GetScale(); //creature->GetOrientation().ToMatrix() *
				}

			}
		}

		return partpos;
	}

	// Open a model resource and find if a property key exists in the file
	bool HasModelKeyValue(const ResourceKey& modelKey, const uint32_t property)
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
	ResourceKey GetModelKeyValue(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		ResourceKey key;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			bool test = App::Property::GetKey(mpPropList.get(), property, key);
			return key;
		}
		else { return {}; }
	}

	ResourceKey GetModelParentKey(const ResourceKey& modelKey) {
		return GetModelKeyValue(modelKey, 0x00B2CCCB);
	}

	// Open a model resource and find text entry
	LocalizedString GetModelText(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		LocalizedString string;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			bool test = App::Property::GetText(mpPropList.get(), property, string);
		}
		return string;
	}

	// Open a model resource and find text entry, return as char
	const char16_t* GetModelString16(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		string16& stringa = string16(u"");
		string8& stringb = string8("");
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			bool test = App::Property::GetString16(mpPropList.get(), property, stringa);
			return stringa.c_str();
			//if (!test) {
			//	bool test = App::Property::GetString8(mpPropList.get(), property, stringb);
			//	sprintf(stringa, "%s", src->c_str());
			//	return u""+(stringb.c_str());
			//}
		}
		return u"";
	}

	LocalizedString GetModelBlockName(const ResourceKey& modelKey) {
		return GetModelText(modelKey, 0x8F6FC401);
	}

	// Open a model resource and find if a property uint32 exists in the file
	bool HasModelUInt32Value(const ResourceKey& modelKey, const uint32_t property)
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
	uint32_t GetModelUInt32Value(const ResourceKey& modelKey, const uint32_t property)
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

	// Open a model resource and find a key instance ID from a property
	uint32_t GetModelKeyInstanceID(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		uint32_t instanceID;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			bool test = App::Property::GetKeyInstanceID(mpPropList.get(), property, instanceID);
			return instanceID;
		}
		else { return {}; }
	}

	// Open a model resource and get a float value of a property
	float GetModelFloatValue(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		float value;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			if (App::Property::GetFloat(mpPropList.get(), property, value)) {
				return value;
			}
		}
		return 0.0f;
	}

	// Open a model resource and get an array of vector2 values of a property
	Vector2* GetModelVector2sValue(const ResourceKey& modelKey, const uint32_t property)
	{
		//Initialize some pointers
		Vector2* partLevels;
		size_t numParts;

		PropertyListPtr mpPropList;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			if (App::Property::GetArrayVector2(mpPropList.get(), property, numParts, partLevels)) {
				return partLevels;
			}
		}
		return nullptr;
	}

	// Open a model resource and get a Color value of a colorRGB property
	ColorRGB GetModelColorRGBValue(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		ColorRGB value;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			if (App::Property::GetColorRGB(mpPropList.get(), property, value)) {
				return value;
			}
		}
		return ColorRGB(0, 0, 0);
	}

	// Open a model resource and get an int value of a property
	int GetModelIntValue(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		int value;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			if (App::Property::GetInt32(mpPropList.get(), property, value)) {
				return value;
			}
		}
		return 0;
	}

	// Open a model resource and get a bool value of a property
	bool GetModelBoolValue(const ResourceKey& modelKey, const uint32_t property)
	{
		PropertyListPtr mpPropList;
		bool value;
		if (PropManager.GetPropertyList(modelKey.instanceID, modelKey.groupID, mpPropList))
		{
			if (App::Property::GetBool(mpPropList.get(), property, value)) {
				return value;
			}
		}
		return false;
	}
}