#include "stdafx.h"
#include "HBCommManager.h"
#include "CapabilityChecker.h"
#include <Spore\Editors\BakeManager.h>


cHBCommManager::cHBCommManager() {
	sInstance = this;
	SimulatorSystem.AddStrategy(this, NOUN_ID);
	//WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, SimulatorMessages::kMsgSwitchGameMode);
}

cHBCommManager::~cHBCommManager() {
	mpLastEvent = nullptr;
	mpCommCreature = nullptr;
	mpCommEmpire = nullptr;
	sInstance = nullptr;
}

cHBCommManager* cHBCommManager::sInstance;

cHBCommManager* cHBCommManager::Get()
{
	return sInstance;
}


ResourceKey cHBCommManager::GetCurrentCommEventSpeciesKey() {
	return mpCommEmpire->GetSpeciesProfile()->mSpeciesKey;
}
bool cHBCommManager::IsCurrentCommEventPlayer() {
	auto specieskey = Simulator::GetPlayerEmpire()->GetSpeciesProfile()->mSpeciesKey;
	if (mpCommEmpire) {
		return mpCommEmpire->GetSpeciesProfile()->mSpeciesKey == specieskey;
	}
	return CommManager.mCurrentCommEvent->mpSourceCivilization->mSpeciesKey == specieskey;
}
bool cHBCommManager::IsCurrentCommEventGrox() {
	if (mpCommEmpire) {
		return mpCommEmpire->GetSpeciesProfile()->mSpeciesKey == Common::kGrox;
	}
	return CommManager.mCurrentCommEvent->mpSourceCivilization->mSpeciesKey == Common::kGrox;
}

void cHBCommManager::OpenCommBckgShopper() {
	auto request = Sporepedia::ShopperRequest(this);
	request.shopperID = id("CommBckgShopper");
	request.Show(request);
}

void cHBCommManager::OnShopperAccept(const ResourceKey& selection)
{
	if (selection == ResourceKey()) { return; }
	mPlayerCommBckg = selection;
	mPlayerID = GetPlayerEmpireID();

	// Play reaction dialog if in space game
	if (IsSpaceGame()) {
		ShowCommEventBckgReact();
	}
	else if (CommManager.IsCommScreenActive() && IsCurrentCommEventPlayer()) {
		HBCommManager.ApplyCommBackground();
	}
}
void cHBCommManager::ShowCommEventBckgReact() {
	cPlanetRecord* planetRecord = Simulator::GetActivePlanetRecord();
	PlanetID planetID = 0;
	if (planetRecord) {
		planetID = planetRecord->GetID();
	};
	CommManager.ShowCommEvent(CommManager.CreateSpaceCommEvent(Simulator::GetPlayerEmpireID(), planetID, id("space_diplomacy_hb"), id("change_background_reaction")));
}

void cHBCommManager::ApplyCommCreatureColor() {
	if (mpCommCreature && CommManager.mCurrentCommEvent && CommManager.mCurrentCommEvent->mpSourceCivilization) {
		mpCommCreature->GetModel()->SetUseColorAsIdentity(true);
		ColorRGB color = CommManager.mCurrentCommEvent->mpSourceCivilization->mPrimaryColor;
		mpCommCreature->GetModel()->SetColor(ColorRGBA(color, 1.0));
	}
}

void cHBCommManager::ApplyCommBackground() {
	if (mPlayerCommBckg != ResourceKey() && CommManager.IsCommScreenActive() && CommManager.mCurrentCommEvent) {
		
		if (mPlayerID != Simulator::GetPlayerEmpireID()) {
			mPlayerCommBckg = {};
			return;
		}

		// Check if the background resource is actually a creation
		if (mPlayerCommBckg.typeID != TypeIDs::Names::prop) {
			PropertyListPtr propList;
			if (!PropManager.GetPropertyList(mPlayerCommBckg.instanceID, mPlayerCommBckg.groupID, propList))
			{
				BakeManager.Bake(mPlayerCommBckg, NULL);
				CommBackgroundBakeCallback();
			}
		}
		else {
			ResourceKey bckgimage = CapabilityChecker::GetModelKeyValue(mPlayerCommBckg, 0xAEB7704E); // sporepediaBackdropImage
			uint32_t bckglight = CapabilityChecker::GetModelKeyValue(mPlayerCommBckg, id("lightingConfig")).instanceID;

			if (bckgimage != ResourceKey()) {
				auto window = CommManager.GetCommBackgroundWindow();
				if (window) {
					object_cast<IImageDrawable>(window->GetDrawable())->SetImageForWindow(window, bckgimage);
				}
				if (bckglight != 0x0 && mpCommCreature) {
					mpCommCreature->GetModelWorld()->GetLightingWorld(0)->SetLightingState(bckglight);
				}
			}
		}
	}
}

// run in a loop until the model is baked.
void cHBCommManager::CommBackgroundBakeCallback() {
	PropertyListPtr propList;
	// Not baked, repeat until done
	if (!PropManager.GetPropertyList(mPlayerCommBckg.instanceID, mPlayerCommBckg.groupID, propList))
	{
		App::ScheduleTask(this, &cHBCommManager::CommBackgroundBakeCallback, 0.1f);
	}
	// Baked successfully, add to comm window
	else {
		ApplyCommBackgroundCreation();
	}
}

void cHBCommManager::ApplyCommBackgroundCreation() {
	if (mPlayerID != Simulator::GetPlayerEmpireID()) {
		mPlayerCommBckg = {};
		return;
	}
	// Comm
	auto commworld = HBCommManager.mpCommCreature->GetModelWorld();
	auto animworld = HBCommManager.mpCommCreature->GetAnimWorld();

	auto model = commworld->CreateModel(mPlayerCommBckg.instanceID, mPlayerCommBckg.groupID);
	Vector3 offset = Vector3(0, 5.0f, 0);
	model->mTransform.SetOffset(offset);
	model->mTransform.SetScale(0.25);
	commworld->StallUntilLoaded(model);
	commworld->SetInWorld(model, true);
}


void cHBCommManager::AddCreatureToCommWindow(const ResourceKey& key, Vector3 position, Quaternion orientation) {
	if (HBCommManager.mpCommCreature) {

		if (auto creature = HBCommManager.mpCommCreature->mpAnimWorld->LoadCreature(key))
		{
			if (position == Vector3()) { position = HBCommManager.mpCommCreature->mPosition; }
			if (orientation == Quaternion()) { orientation = HBCommManager.mpCommCreature->mOrientation; }

			creature->mPosition = position;
			creature->mOrientation = orientation;

			auto commworld = HBCommManager.mpCommCreature->GetModelWorld();
			commworld->StallUntilLoaded(creature->GetModel());
			commworld->SetInWorld(creature->GetModel(), true);
		}
	}
}

//------------------------------------------------------------

bool cHBCommManager::HandleMessage(uint32_t messageID, void* msg)
{
	if (messageID == SimulatorMessages::kMsgSwitchGameMode)
	{
		HBCommManager.mpCommCreature = nullptr;
		HBCommManager.mpCommEmpire = nullptr;
	}
	return false;
}

/*
int cHBCommManager::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

// TODO: this
bool cHBCommManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsSpaceGame() && !IsCivGame()) { return false; }

	if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();

	}
	return false;
}*/

//-----------------
// ATTACH DETOURS
void cHBCommManager::AttachDetours() {
	Comms_CreateSpaceCommEvent_detour::attach(GetAddress(Simulator::cCommManager, CreateSpaceCommEvent));
	Comms_ShowCommEvent_detour::attach(GetAddress(Simulator::cCommManager, ShowCommEvent));
	Comms_HandleSpaceCommAction_detour::attach(GetAddress(Simulator::cCommManager, HandleSpaceCommAction));
	Comms_HandleCivCommAction_detour::attach(GetAddress(Simulator::cCommManager, HandleCivCommAction));
}



Simulator::Attribute cHBCommManager::ATTRIBUTES[] = {
	// Add more attributes here
	SimAttribute(cHBCommManager,mPlayerCommBckg,1),
	SimAttribute(cHBCommManager,mPlayerID,2),
	// This one must always be at the end
	Simulator::Attribute()
};

// For internal use, do not modify.
int cHBCommManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cHBCommManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cHBCommManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(cHBCommManager);
	return nullptr;
}

void cHBCommManager::Initialize() {
}

void cHBCommManager::Dispose() {
}

const char* cHBCommManager::GetName() const {
	return "Valla_CreatureOverhaul::cHBCommManager";
}

bool cHBCommManager::Write(Simulator::ISerializerStream* stream)
{
	if (mPlayerID != Simulator::GetPlayerEmpireID()) {
		mPlayerCommBckg = {};
	}
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool cHBCommManager::Read(Simulator::ISerializerStream* stream)
{
	mPlayerCommBckg = ResourceKey();
	mPlayerID = 0x0;
	return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
}

void cHBCommManager::Update(int deltaTime, int deltaGameTime) {

}