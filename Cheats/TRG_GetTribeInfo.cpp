#include "stdafx.h"
#include "TRG_GetTribeInfo.h"

TRG_GetTribeInfo::TRG_GetTribeInfo()
{
}


TRG_GetTribeInfo::~TRG_GetTribeInfo()
{
}


void TRG_GetTribeInfo::ParseLine(const ArgScript::Line& line)
{
	if (Simulator::IsTribeGame()) {
		cTribePtr tribe = GameNounManager.GetPlayerTribe();
		App::ConsolePrintF("--- Tribe info for 0x%x ---", tribe->GetCommunityName());
		App::ConsolePrintF("Identity Color ID: %x",tribe->mIDColorID);
		App::ConsolePrintF("Identity Cached Color: %x",tribe->mCachedColor);
		App::ConsolePrintF("Archetype: %i",tribe->mTribeArchetype);
		App::ConsolePrintF("Tribe Upgrade level: %x",tribe->mUpgradeLevel);
		App::ConsolePrintF("Population size: %x",tribe->GetPopulationCount());
		App::ConsolePrintF("Population capacity: %x",tribe->mCommunityMembers.capacity());
		App::ConsolePrintF("Purchased Tools: %i", tribe->mPurchasedTools);
		App::ConsolePrintF("Tribe Zone Radius: %x",tribe->mZoningRadius);
		App::ConsolePrintF("Chieftain: 0x%x", tribe->GetLeaderCitizen()->mCreatureName);
		App::ConsolePrintF("Chieftain Color: 0x%x", tribe->GetLeaderCitizen()->GetIdentityColor());
		App::ConsolePrintF("Tribe Members: %x", tribe->GetTribeMembers().size());
		for (auto creature : tribe->GetTribeMembers()) {
			App::ConsolePrintF("	0x%x", creature->mCreatureName);
		}
		App::ConsolePrintF("----------------------------");
		App::ConsolePrintF("Tribe Layout: 0x%x",tribe->mTribeLayout);
		App::ConsolePrintF("----------------------------");
		int slotnum = 0;
		for (auto slot : tribe->mTribeLayout.mSlots) {
			slotnum += 1;
			App::ConsolePrintF("Slot %i data below:", slotnum);
			App::ConsolePrintF("	Occupied:  %b", slot.mIsOccupied);
			App::ConsolePrintF("	Object:  %x", slot.mpObject);
			App::ConsolePrintF("	Position:  (%f, %f, %f)", slot.mPosition.x, slot.mPosition.y, slot.mPosition.z);
			App::ConsolePrintF("	field_8:  0x%x", slot.field_8);
			App::ConsolePrintF("\n ");
		}
		App::ConsolePrintF("----------------------------");
		App::ConsolePrintF("Total Food: %i",tribe->GetTotalFood());
		App::ConsolePrintF("Food in Egg Pen: %i",tribe->mEggPenFoodValue);
		App::ConsolePrintF("Tools Count: %i",tribe->mPurchasedTools);
		App::ConsolePrintF("----------------------------");
		int toolnum = 0;
		for (auto tool : tribe->GetTools()) {
			toolnum += 1;
			App::ConsolePrintF("Tool %i data below:", toolnum);
			App::ConsolePrintF("	ModelKey:  0x%x", tool->GetModelKey().instanceID);
			App::ConsolePrintF("	Health:  %f / %f", tool->mHealthPoints, tool->mMaxHealthPoints);
			App::ConsolePrintF("	mToolType:  0x%x", tool->mToolType);
			App::ConsolePrintF("	Tool Type:  %i", tool->GetToolType());
			App::ConsolePrintF("	Tool Class:  %i", tool->GetToolClass());

			App::ConsolePrintF("	field_1EC:  0x%x", tool->field_1EC);
			App::ConsolePrintF("	field_1F0:  %i", tool->field_1F0);
			App::ConsolePrintF("	field_1F4:  %i", tool->field_1F4);
			App::ConsolePrintF("	field_240:  %i", tool->field_240);
			App::ConsolePrintF("	field_244:  %i", tool->field_244);
			App::ConsolePrintF("\n ");
		}
	}

	if (Simulator::IsCivGame()) {
		auto civ = GameNounManager.GetPlayerCivilization();
		cCityPtr city = civ->mCities[0];

		App::ConsolePrintF("--- City info for 0x%x ---", city->GetCommunityName());
		App::ConsolePrintF("Is Small City: %b", city->mbSmallCity);
		App::ConsolePrintF("Species Key: 0x%x, 0x%x", city->mSpeciesKey.instanceID, city->mpCivilization->mSpeciesKey);
		// DEBUG
		//city->mpCivilization->mSpeciesKey = ResourceKey(0x06577404, TypeIDs::Names::crt, 0x40626200);
		//
		App::ConsolePrintF("Housing Amount: %i", city->mHousingAmount);
		App::ConsolePrintF("Vehicle Specialty: %i", city->mVehicleSpecialty);
		App::ConsolePrintF("----------------------------");
		App::ConsolePrintF("Building Layout: 0x%x", city->mBuildingsLayout);
		App::ConsolePrintF("----------------------------");
		int slotnum = 0;
		for (auto slot : city->mBuildingsLayout.mSlots) {
			slotnum += 1;
			App::ConsolePrintF("Slot %i data below:", slotnum);
			App::ConsolePrintF("	Occupied:  %b", slot.mIsOccupied);
			App::ConsolePrintF("	Object:  %x", slot.mpObject);
			App::ConsolePrintF("	Position:  (%f, %f, %f)", slot.mPosition.x, slot.mPosition.y, slot.mPosition.z);
			App::ConsolePrintF("	field_8:  0x%x", slot.field_8);
			App::ConsolePrintF("\n ");
		}
		App::ConsolePrintF("----------------------------");
		App::ConsolePrintF("Turrets Layout: 0x%x", city->mTurretsLayout);
		App::ConsolePrintF("----------------------------");
		slotnum = 0;
		for (auto slot : city->mTurretsLayout.mSlots) {
			slotnum += 1;
			App::ConsolePrintF("Slot %i data below:", slotnum);
			App::ConsolePrintF("	Occupied:  %b", slot.mIsOccupied);
			App::ConsolePrintF("	Object:  %x", slot.mpObject);
			App::ConsolePrintF("	Position:  (%f, %f, %f)", slot.mPosition.x, slot.mPosition.y, slot.mPosition.z);
			App::ConsolePrintF("	field_8:  0x%x", slot.field_8);
			App::ConsolePrintF("\n ");
		}
		App::ConsolePrintF("----------------------------");
		App::ConsolePrintF("Decor Layout: 0x%x", city->mDecorationsLayout);
		App::ConsolePrintF("----------------------------");
		slotnum = 0;
		for (auto slot : city->mDecorationsLayout.mSlots) {
			slotnum += 1;
			App::ConsolePrintF("Slot %i data below:", slotnum);
			App::ConsolePrintF("	Occupied:  %b", slot.mIsOccupied);
			App::ConsolePrintF("	Object:  %x", slot.mpObject);
			App::ConsolePrintF("	Position:  (%f, %f, %f)", slot.mPosition.x, slot.mPosition.y, slot.mPosition.z);
			App::ConsolePrintF("	field_8:  0x%x", slot.field_8);
			App::ConsolePrintF("\n ");
		}
	}
	
}

const char* TRG_GetTribeInfo::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Prints debug info about the player's tribe.";
	}
	else {
		return "TribeInfo: EPrints debug info about the player's tribe.";
	}
}
