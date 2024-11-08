#include "stdafx.h"
#include "BuildCRG.h"
#include <Spore/Simulator/cCreatureGameData.h>

cCreatureAnimalPtr avatar;
cHerdPtr herd;
cNestPtr nest;
ResourceKey hutmodel; //= ResourceKey(id("he_carn_unpack1"), id("hutstyles"), TypeIDs::prop);
cSpatialObjectPtr buildobject;
int type = 0;

BuildCRG::BuildCRG()
{
	ResourceKey::Parse(hutmodel, u"tt_gather_home_03.prop", 0, id("tribaltools"));
}


BuildCRG::~BuildCRG()
{
}


int BuildCRG::AddRef()
{
	return DefaultRefCounted::AddRef();
}

int BuildCRG::Release()
{
	return DefaultRefCounted::Release();
}


void BuildCRG::SetNestModel()
{
	//pNest->SetModelKey(pModel);
	if (nest) {
		nest->SetModelKey(hutmodel);
	}
}

using namespace Simulator;
void BuildCRG::ParseLine(const ArgScript::Line& line)
{
	mParameter = line.GetArguments(1)[0];
	avatar = GameNounManager.GetAvatar();
	// Set species profile
	cSpeciesProfile* speciesProfile;
	cHerdPtr herdAvatar;
	Vector3 pos;
	if (avatar) {
		speciesProfile = avatar->mpSpeciesProfile;
		herdAvatar = avatar->mHerd;
		pos = avatar->GetPosition();
	}
	else if (IsTribeGame()) {
		speciesProfile = GameNounManager.GetPlayerTribe()->mTribeMembers[0]->mpSpeciesProfile;
		herdAvatar = GameNounManager.GetPlayerTribe()->mpDomesticatedAnimalsHerd;
		pos = GameViewManager.GetWorldMousePosition();
	}
	else {
		speciesProfile = SpeciesManager.GetSpeciesProfile(GameNounManager.GetPlayerCivilization()->mSpeciesKey);
		herdAvatar = nullptr;
		pos = GameViewManager.GetWorldMousePosition();
	}

	if (CompareStrings(mParameter, "Nest")){
		//herd = GameNounManager.CreateHerd(avatar->GetPosition(), avatar->mpSpeciesProfile, 4, false, 0, true);
		// grox test:
		//Simulator::cSpeciesProfile* grox = SpeciesManager.GetSpeciesProfile(ResourceKey(0x06577404, TypeIDs::Names::crt, 0x40626200));
		herd = GameNounManager.CreateHerd(pos, speciesProfile, 4, false, 9, true);

		type = 1;
		clocktest.SetMode(Clock::Mode::Milliseconds);
		clocktest.Start();
		//herd->mHitpointOverride = 0;
		//Simulator::ScheduleTask(this, &BuildCRG::SetNestModel, 0.1f);
	}
	else if (CompareStrings(mParameter, "Hut")) {
		//herd = GameNounManager.CreateHerd(avatar->GetPosition(), avatar->mpSpeciesProfile, 2, false, 0, true);
		//nest = herd->mpNest;
		nest = GameNounManager.CreateNest(pos, herdAvatar.get());
		
		clocktest.SetMode(Clock::Mode::Milliseconds);
		clocktest.Start();
		type = 0;
		if (avatar) {
			avatar->SetCurrentBrainLevel(5);
			avatar->mMaxEnergy = 2000;
			avatar->mEnergy = 2000;
			avatar->mHunger -= 5;
		}
	}
	else if (CompareStrings(mParameter, "Tribe")) {
		cTribe* tribe = Simulator::SpawnNpcTribe(pos, 3, 1, 1, true, speciesProfile);
	}
	// TODO
	else if (CompareStrings(mParameter, "Camp")) {
		cTribe* tribe = Simulator::SpawnNpcTribe(pos, 3, 1, 1, true, speciesProfile);
	}
	else {
		SetNestModel();
	}
}

bool BuildCRG::CompareStrings(const char* str1, const char* str2)
{
	string16 str1_lower = string16(string16::CtorConvert(), str1);
	string16 str2_lower = string16(string16::CtorConvert(), str2);
	str1_lower.make_lower();
	str2_lower.make_lower();;
	return (str1_lower == str2_lower);
}

const char* BuildCRG::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat does something.";
	}
	else {
		return "BuildCRG: Elaborate description of what this cheat does.";
	}
}
