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
	App::AddUpdateFunction(this);
	ResourceKey::Parse(hutmodel, u"tt_gather_home_03.prop", 0, id("tribaltools"));
}


BuildCRG::~BuildCRG()
{
}

void BuildCRG::Update()
{
	avatar = GameNounManager.GetAvatar();

	//SporeDebugPrint("%b, %i", clocktest.IsRunning(), clocktest.GetElapsedTime());
	if (clocktest.GetElapsedTime() > 50) {
		switch (type)
		{
			case 0: 
				clocktest.Reset();
				//buildobject->Teleport(avatar->GetPosition(), avatar->GetOrientation());
				SetNestModel();
			case 1:
				if (clocktest.GetElapsedTime() > 1000) {
					clocktest.Reset();
					//herd->mpHerdMom->SetCurrentBrainLevel(5);
					for (cCreatureAnimalPtr animal : herd->mHerd) {
						if (animal) {
							animal->SetCurrentBrainLevel(5);
						}
					}
					herd->mHerd[0]->SetCurrentBrainLevel(5);
					herd->mHerd[1]->SetCurrentBrainLevel(5);
					herd->mHerd[2]->SetCurrentBrainLevel(5);
					//herd->mHerd[2]->mbIsDiseased = true;
					herd->mHerd[3]->SetCurrentBrainLevel(5);

					herd->mHerd[3]->mbColorIsIdentity = true;
					//herd->mHerd[3]->SetIdentityColor(ColorRGB(2.0f, 2.0f, 2.0f));

					//avatar->mbIsDiseased = true;
				}
				
		}
		
	}
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
	nest->SetModelKey(hutmodel);
}

void BuildCRG::ParseLine(const ArgScript::Line& line)
{
	mParameter = line.GetArguments(1)[0];
	//
	avatar = GameNounManager.GetAvatar();
	//
	if (CompareStrings(mParameter, "Nest")){
		//herd = GameNounManager.CreateHerd(avatar->GetPosition(), avatar->mpSpeciesProfile, 4, false, 0, true);
		// grox test:
		Simulator::cSpeciesProfile* grox = SpeciesManager.GetSpeciesProfile(ResourceKey(0x06577404, TypeIDs::Names::crt, 0x40626200));
		herd = GameNounManager.CreateHerd(avatar->GetPosition(), grox, 4, false, 9, true);

		type = 1;
		clocktest.SetMode(Clock::Mode::Milliseconds);
		clocktest.Start();
		herd->mHitpointOverride = 0;
		//herd->mpHerdMom = avatar;
		//herd->mpEggLayer = avatar;
	}
	else if (CompareStrings(mParameter, "Hut")) {
		//herd = GameNounManager.CreateHerd(avatar->GetPosition(), avatar->mpSpeciesProfile, 2, false, 0, true);
		//nest = herd->mpNest;
		nest = GameNounManager.CreateNest(avatar->ToSpatialObject()->GetPosition(), avatar->mHerd.get());
		
		clocktest.SetMode(Clock::Mode::Milliseconds);
		clocktest.Start();
		type = 0;
		avatar->SetCurrentBrainLevel(5);
		avatar->mMaxEnergy = 2000;
		avatar->mEnergy = 2000;
		avatar->mHunger -= 5;
	}
	else if (CompareStrings(mParameter, "Tribe")) {
		App::ConsolePrintF("Tribe Spawn now");
		auto test = GameNounManager.CreateInstance(Simulator::kRock);
		buildobject = object_cast<Simulator::cSpatialObject>(test);
		clocktest.SetMode(Clock::Mode::Milliseconds);
		clocktest.Start();
		buildobject->SetModelKey(avatar->GetModelKey()); // test because my normal model isnt working
		buildobject->SetScale(1);
		buildobject->mbFixed = true;
		buildobject->mbPickable = true;
		buildobject->mbKeepPinnedToPlanet = false;
		buildobject->mbIsTangible = true;
		buildobject->mbIsGhost = false;
		buildobject->mbTransformDirty = false;
		buildobject->Teleport(avatar->GetPosition(), avatar->GetOrientation());
		//obj->Teleport(mTrans.GetOffset(), Math::Quaternion::FromEuler(rot));
	}
	else {
		SetNestModel();
		//nest->SetModelKey(hutmodel);
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
