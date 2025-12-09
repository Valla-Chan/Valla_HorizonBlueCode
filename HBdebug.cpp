#include "stdafx.h"
#include "HBdebug.h"
#include <Spore\Editors\Editor.h>
#include <Spore\Editors\BakeManager.h>
#include <Spore\Simulator\cCreatureGameData.h>
#include <Spore\Simulator\SubSystem\CommManager.h>
#include "HBCommManager.h"
#include "CityMemberManager.h"
#include "Common.h"

using namespace Simulator;
using namespace Cell;

HBdebug::HBdebug()
{
}


HBdebug::~HBdebug()
{
}

//-----------------------------------------------------------------------------------

Vector3 camPos = Vector3(0, 0, 0);
Vector3 camDir = Vector3(0, 0, 0);
void TraceHeader() {
	auto pViewer = CameraManager.GetViewer();

	// Get vector to the center of the screen.
	auto windowArea = WindowManager.GetMainWindow()->GetArea();
	pViewer->GetCameraToMouse(camPos, camDir);
}

cCreatureCitizenPtr TraceHitCitizen() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto citizen = object_cast<cCreatureCitizen>(object);
			if (citizen && citizen->mpOwnerCity->IsPlayerOwned()) {
				return citizen;
			}
		}
	}
	return nullptr;
}

cCreatureAnimalPtr TraceHitAnimal() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto animal = object_cast<cCreatureAnimal>(object);
			if (animal) {
				return animal;
			}
		}
	}
	return nullptr;
}

// not currently working
cGamePlantPtr TraceHitPlant() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			auto plant = object_cast<cGamePlant>(object);
			return plant;
		}
	}
	return nullptr;
}

cSpatialObjectPtr TraceHitSpatial() {
	TraceHeader();
	vector<cSpatialObjectPtr> raycastObjects;

	if (GameViewManager.RaycastAll(camPos, camPos + (camDir * 900.0f), raycastObjects, true)) {
		for (auto object : raycastObjects) {
			return object;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------

void HBdebug::StoreData() {
// Some common vars we might want to use
//--------------------------------------------------------------------

// Objects and Players
	avatar = GameNounManager.GetAvatar();
	player = GetPlayer();
	/* herd */ if (avatar) { herd = avatar->mHerd; }
	/* nest */ if (herd) { nest = herd->mpNest; }
	tribe = GameNounManager.GetPlayerTribe();
	/* chieftain */ if (tribe) { chieftain = tribe->GetLeaderCitizen(); }
	hovered = GameViewManager.GetHoveredObject();
	hoveredAnimal = object_cast<cCreatureAnimal>(hovered);
	hoveredCitizen = object_cast<cCreatureCitizen>(hovered);
	hoveredCombatant = object_cast<cCombatant>(hovered);

// UI
	window = WindowManager.GetMainWindow();
// Editor
	editor = GetEditor();
	editorModel = Editor.GetEditorModel();

// CRG part unlocks
	/* crgparts */ if (player) { crgparts = player->mpCRGItems; }

// Planet
	planetRecord = Simulator::GetActivePlanetRecord();
	nests = GetData<cNest>();
	empire = Simulator::GetPlayerEmpire();
	if (planetRecord) civilization = GameNounManager.GetPlayerCivilization(); //exists when on planet surface
	cities = GetData<cCity>();
/* firstCity */ if (!cities.empty()) { firstCity = cities[0]; }
	if (IsSpaceGame()) ship = Simulator::GetPlayerUFO();

}

//-----------------------------------------------------------------------------------

UILayout commUIlayout;
UILayout spaceUIlayout;

void HBdebug::ParseLine(const ArgScript::Line& line)
{
	StoreData();

	// Your code here:
	//------------------------------------------
	//OpenShopper();
	//player->mpCRGItems = nullptr; // NOTE: when this is made nullptr, the new API works in existing saves?
	//MessageManager.MessageSend(id("UnlockPart"), nullptr);
	//SporeDebugPrint("0x%x", CityMemberManager.GetCurrentGameMode());
	//crgparts->field_C = false;
	//crgparts->UnlockPart(id("ce_mouth_amphibian_omnivore_04"), 0x40626000);
	//crgparts->UnlockPart(id("ce_mouth_mandible_carnivore_04"), 0x40626000);
	//crgparts->AddUnlockableItemFromProp(ResourceKey(id("ce_mouth_mandible_carnivore_01"), prop, 0x40626000), 0x887c91bb, 1, 0, 0);
	//crgparts->AddUnlockableItemFromProp(ResourceKey(id("ce_mouth_mandible_carnivore_02"), prop, 0x40626000), 0x887c91bb, 1, 1, 0);
	//crgparts->AddUnlockableItemFromProp(ResourceKey(id("ce_mouth_mandible_carnivore_03"), prop, 0x40626000), 0x887c91bb, 1, 2, 0);
	//crgparts->AddUnlockableItemFromProp(ResourceKey(id("ce_mouth_mandible_carnivore_04"), prop, 0x40626000), 0x887c91bb, 1, 3 ,0);
	//crgparts->sub_594010();
	//auto thing = crgparts->field_6D8C;
	/*
	for (auto item : crgparts->mUnlockedItems) {
		SporeDebugPrint("--------------");
		vector<int> dst;
		crgparts->sub_597390(dst, item, 1);
		for (auto idx : dst) {
			SporeDebugPrint("0x%x", idx);
		}
	}*/

	// TODO: try unlocking all mouths up to one i want, then re-locking the ones i dont want.
	App::CreatureModeStrategies::UnlockPart action = { GameNounManager.GetAvatar(), 0, 0 };

	//------------------------------------------
	SporeDebugPrint("HBdebug done executing.");
}



const char* HBdebug::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "Horizon Blue Debugging Placeholder. This cheat does nothing unless modified by a developer.";
	}
	else {
		return "HBdebug: Debugging Placeholder. This cheat does nothing unless modified by a developer.";
	}
}


//-------------------------------------------------------------------------------------------------------------
// Sporepedia

void HBdebug::OpenShopper() {
	auto request = Sporepedia::ShopperRequest(this);
	request.shopperID = id("noadv");
	request.Show(request);
}

void HBdebug::OnShopperAccept(const ResourceKey& selection)
{
	if (selection == ResourceKey()) { return; }

	PropertyListPtr propList;
	if (!PropManager.GetPropertyList(selection.instanceID, selection.groupID, propList))
	{
		BakeManager.Bake(selection, NULL);
	}

	// Comm
	auto commworld = HBCommManager.mpCommCreature->GetModelWorld();
	auto animworld = HBCommManager.mpCommCreature->GetAnimWorld();

	auto model = commworld->CreateModel(selection.instanceID, selection.groupID);
	Vector3 offset = Vector3(0.07f, 5.0f, -0.76f);
	model->mTransform.SetOffset(offset);
	model->mTransform.SetScale(0.3f);
	model->mTransform.SetRotation(Quaternion::FromEuler(Vector3(0, 0, ToRadians(180))));
	commworld->StallUntilLoaded(model);
	commworld->SetInWorld(model, true);
}

//-------------------------------------------------------------------------------------------------------------
// Premade Funcs

void CitizensUseMissileOnTarget(vector<cCreatureCitizenPtr> members, cCombatant* target) {
	for (cCreatureCitizenPtr member : members) {
		member->field_1001 = false;
		member->field_1002 = false;
		member->mbInvincible = true;

		member->DoAction(kCitizenActionAttack, target->ToGameData());
		member->SetTarget(target);
		member->mpTarget = target;
		member->mpCombatantTarget = target;

		ResourceKey abilitykey = ResourceKey(id("Adventurer_Missile1"), TypeIDs::Names::prop, id("CreatureAbilities"));

		int abilityindex = -1;
		bool abilityowned = false;
		for (int i = member->mpSpeciesProfile->mAbilities.size() - 1; i >= 0; i--)
		{
			if (member->mpSpeciesProfile->mAbilities[i]->mpPropList->GetResourceKey().instanceID == abilitykey.instanceID)
			{
				abilityindex = i;
				abilityowned = true;
				break;
			}
		}
		if (!abilityowned)
		{
			cCreatureAbility* ability = new(cCreatureAbility);
			PropertyListPtr mpPropList;
			if (PropManager.GetPropertyList(abilitykey.instanceID, abilitykey.groupID, mpPropList))
			{
				cCreatureAbility::Parse(ability, mpPropList.get());

				// add the new ability to the species profile
				member->mpSpeciesProfile->mAbilities.push_back(ability);
				abilityindex = member->mpSpeciesProfile->mAbilities.size() - 1;
				member->mRechargingAbilityBits[abilityindex] = 0;
				member->mInUseAbilityBits[abilityindex] = 0;

				// AbilityState
				auto a = Simulator::cAbilityState(member->mAbilityStates[abilityindex - 1]);
				a.field_08 = ResourceID(member->mAbilityStates[abilityindex - 1].field_08);
				a.field_08.instanceID = abilitykey.instanceID;
				a.field_08.groupID = abilitykey.groupID;
				member->mAbilityStates.push_back(a);

			}

		}
		if (abilityindex > 0) {
			member->PlayAbility(abilityindex);
		}
	}
}

void HBdebug::CiviliansAttackHovered() {
	auto city = Common::GetNearestCity();
	if (city && hoveredCombatant) {
		CitizensUseMissileOnTarget(CityMemberManager.GetCityCitizens(city), hoveredCombatant.get());
	}
}


void HBdebug::TribesAttackShip() {
	vector<cCreatureCitizenPtr> members;
	for (auto tribeitem : Simulator::GetData<Simulator::cTribe>()) {
		for (auto member : tribeitem->GetTribeMembers()) {
			members.push_back(member);
		}
	}
	CitizensUseMissileOnTarget(members, ship.get());
}
