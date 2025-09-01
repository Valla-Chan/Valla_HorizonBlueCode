#include "stdafx.h"
#include "HBCommManager.h"
#include "CapabilityChecker.h"


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

void cHBCommManager::AddCreatureToCommWindow(const ResourceKey& key, Vector3 position, Quaternion orientation) {
	if (HBCommManager.mpCommCreature) {

		if (auto creature = HBCommManager.mpCommCreature->mpAnimWorld->LoadCreature(key))
		{
			if (position == Vector3()) { position = HBCommManager.mpCommCreature->mPosition; }
			if (orientation == Quaternion()) { orientation = HBCommManager.mpCommCreature->mOrientation; }

			creature->mPosition = position;
			creature->mOrientation = orientation;

			HBCommManager.mpCommCreature->GetModel()->GetModelWorld()->StallUntilLoaded(creature->GetModel());
			HBCommManager.mpCommCreature->GetModel()->GetModelWorld()->SetInWorld(creature->GetModel(), true);
		}
	}
}

//------------------------------------------------------------

bool cHBCommManager::HandleMessage(uint32_t messageID, void* msg)
{
	if (messageID == SimulatorMessages::kMsgSwitchGameMode)
	{
		if (mPlayerID != Simulator::GetPlayerEmpireID()) {
			mPlayerCommBckg = {};
		}
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
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool cHBCommManager::Read(Simulator::ISerializerStream* stream)
{
	mPlayerCommBckg = ResourceKey();
	return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
}

void cHBCommManager::Update(int deltaTime, int deltaGameTime) {

}