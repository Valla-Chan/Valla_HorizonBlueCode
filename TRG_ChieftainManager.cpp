#include "stdafx.h"
#include "TRG_ChieftainManager.h"
#include "CapabilityChecker.h"
#include <Spore\Simulator\SimulatorMessages.h>
#include <Spore\Editors\BakeManager.h>

TRG_ChieftainManager::TRG_ChieftainManager()
{
	sInstance = this;
	SimulatorSystem.AddStrategy(this, NOUN_ID);
	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, id("TRG_GetTool")); // listen for anim event
	MessageManager.AddListener(this, id("UpdateStaffIcon")); // listen for anim event
}


TRG_ChieftainManager::~TRG_ChieftainManager()
{
	sInstance = nullptr;
}

TRG_ChieftainManager* TRG_ChieftainManager::sInstance;

TRG_ChieftainManager* TRG_ChieftainManager::Get()
{
	return sInstance;
}

void TRG_ChieftainManager::Reset() {
	last_tribe_count = 0;
	tribe_count = 0;
	tribe_staff_id = -1;
}

bool TRG_ChieftainManager::HandleMessage(uint32_t messageID, void* msg)
{
	if (!IsTribeGame()) { return nullptr; }


	if (messageID == id("TRG_GetTool"))
	{
		auto newMsg = (Anim::AnimationMessage*)msg; //use casting to get an Anim::AnimationMessage from the msg variable
		cCreatureCitizen* creature = GetAnimCreatureOwner(newMsg->pCreature);
		if (creature && IsCreatureTribeLeader(creature)) {
			AddChiefToQueue(creature);
			//SporeDebugPrint(" - Chieftain has grabbed or put away tool.");
		}
	}
	else if (messageID == id("UpdateStaffIcon")) {
		UpdateStaffIcon();
		//UpdateStaffModel();
	}
	return false;
}



cCreatureCitizen* TRG_ChieftainManager::GetAnimCreatureOwner(const AnimatedCreaturePtr& animcreature) {
	cCombatantPtr target = nullptr;
	for (auto creature : Simulator::GetData<Simulator::cCreatureCitizen>()) { //loop through all creatures
		if (creature->mpAnimatedCreature.get() == animcreature) //if the current creature is the owner of the AnimatedCreature that triggered the event, do the following
		{
			return creature.get(); //set the crt pointer to the current creature
		}

	}
	return nullptr;

}

cCreatureCitizen* TRG_ChieftainManager::GetNearestTribalLeader(Vector3 pos, const float within) const {
	if (!IsTribeGame()) { return nullptr; }

	float last_distance = within;
	cCreatureCitizen* closest_leader = nullptr;

	auto tribes = Simulator::GetData<Simulator::cTribe>();
	for (auto tribe : tribes) {
		cCreatureCitizen* leader = tribe->GetLeaderCitizen();
		if (!leader) { return nullptr; }
		float dist = (leader->GetPosition() - pos).SquaredLength();
		if (dist < last_distance) {
			closest_leader = leader;
		}
	}
	return closest_leader;
}

bool TRG_ChieftainManager::IsCreatureTribeLeader(const cCreatureCitizenPtr& creature) {
	// compare citizen to each tribal leaders till we find a match
	auto tribes = Simulator::GetData<Simulator::cTribe>();
	for (auto tribe : tribes) {
		if (creature == tribe->GetLeaderCitizen()) {
			return true;
		}
	}
	return false;
}

int TRG_ChieftainManager::GetChiefDietValue(cCreatureCitizenPtr chief, bool use_override) {
	int value = 0;
	if (!IsTribeGame()) { return value; }
	last_tribe_count = tribe_count;
	auto tribes = Simulator::GetData<Simulator::cTribe>();
	tribe_count = tribes.size();

	if (tribe_count == 0) { return 0; }

	// if no chief, get the one from the most recently spawned tribe
	if (!chief) {
		// a new tribe has spawned since last time
		if (tribe_count > last_tribe_count) {
			chief = tribes[tribe_count - 1]->GetLeaderCitizen();
		}
		// a new tribe has not spawned, meaning that the effect has probably just popped in via distance culling.
		else {
			chief = GetNearestTribalLeader(GameViewManager.GetWorldMousePosition(),4096);
			// fall back to last tribe if this is null.
			if (!chief) {
				auto tribe = tribes[tribe_count - 1];
				if (tribe) {
					chief = tribe->GetLeaderCitizen();
				}
				
			}
		}
	}
	if (!chief) { return -1; }
	if (use_override && chief->mpOwnerTribe == GameNounManager.GetPlayerTribe() && mStaffRes.instanceID != 0x0) {
		return 4; // use override
	}

	int carn = CapabilityChecker.GetCapabilityLevel(chief, 0x022E7847);
	int herb = CapabilityChecker.GetCapabilityLevel(chief, 0x022E785C);
	int grasper = CapabilityChecker.GetCapabilityLevel(chief, 0xB00F0FE2);
	if (herb) {
		value = herb + carn;
	}
	if (!grasper) {
		value = 3;
	}
	return value;
}

// Add a chief creature's diet value to the queue
void TRG_ChieftainManager::AddChiefToQueue(const cCreatureCitizenPtr chief) {
	tribe_staff_id = GetChiefDietValue(chief);
}

int TRG_ChieftainManager::NextQueueItem() {
	int value = tribe_staff_id;
	if (value == -1) {
		value = GetChiefDietValue(nullptr);
	}
	return value;
}

uint32_t TRG_ChieftainManager::ConvertStaffEffectID(uint32_t instanceId) {
	uint32_t staff_id = 0x0;
	int attachment = GetStaffAttachmentIndex(instanceId);
	int dietvalue = 0;
	if (attachment > -1) {
		dietvalue = NextQueueItem();
		staff_id = GetStaffID(dietvalue, attachment);
	}
	return staff_id;
}

// 0 = reg  1 = fish  2 = seaweed
int TRG_ChieftainManager::GetStaffAttachmentIndex(uint32_t instanceId) const {
	if (instanceId == id("trg_chieftain_staff")) { return 0; }
	if (instanceId == id("trg_chieftain_staff_fish")) { return 1; }
	if (instanceId == id("trg_chieftain_staff_seaweed")) { return 2; }
	return -1;
}

uint32_t TRG_ChieftainManager::GetStaffID(int diet, int attachment) const {
	if (attachment > -1) {
		//SporeDebugPrint(" - Accessing Chieftain diet value:  %x", dietvalue);

		switch (diet) {
			// (case 0 is carnivore aka default)
			// Herb
			case 1:
				if (attachment == 0)
					return id("trg_chieftain_staff_herb");
				if (attachment == 1)
					return id("trg_chieftain_staff_herb_fish");
				if (attachment == 2)
					return id("trg_chieftain_staff_herb_seaweed");
				break;
			// Omni
			case 2:
				if (attachment == 0)
					return id("trg_chieftain_staff_omni");
				if (attachment == 1)
					return id("trg_chieftain_staff_omni_fish");
				if (attachment == 2)
					return id("trg_chieftain_staff_omni_seaweed");
				break;
			// Mouth
			case 3:
				if (attachment == 0)
					return id("trg_chieftain_staff_mouth");
				if (attachment == 1)
					return id("trg_chieftain_staff_mouth_fish");
				if (attachment == 2)
					return id("trg_chieftain_staff_mouth_seaweed");
				break;
			// Override
			case 4:
				return GetStaffID(mStaffRes, attachment);
		}
	}
	return 0x0;
}

uint32_t TRG_ChieftainManager::GetStaffID(const ResourceKey &staffres, int attachment) const {
	uint32_t keyid = 0x0;
	switch (attachment) {
		case 0:
			return CapabilityChecker.GetModelKeyValue(staffres, id("tribeStaffEffect")).instanceID;
		case 1:
			keyid = CapabilityChecker.GetModelKeyValue(staffres, id("tribeStaffEffectFish")).instanceID;
			if (keyid == 0x0) {
				return CapabilityChecker.GetModelKeyValue(staffres, id("tribeStaffEffect")).instanceID;
			}
			return keyid;
		case 2:
			keyid = CapabilityChecker.GetModelKeyValue(staffres, id("tribeStaffEffectSeaweed")).instanceID;
			if (keyid == 0x0) {
				return CapabilityChecker.GetModelKeyValue(staffres, id("tribeStaffEffect")).instanceID;
			}
			return keyid;
	}
	return 0x0;
}

void TRG_ChieftainManager::OpenStaffShopper() {
	auto request = Sporepedia::ShopperRequest(this);
	request.shopperID = id("FixedObjectStaffShopper");
	request.Show(request);
}

void TRG_ChieftainManager::OnShopperAccept(const ResourceKey& selection)
{
	if (selection == ResourceKey()) { return; }

	PropertyListPtr propList;
	if (!PropManager.GetPropertyList(selection.instanceID, selection.groupID, propList))
	{
		BakeManager.Bake(selection, NULL);
	}
	// store model and update staff
	mStaffRes = selection;
	mbStaffSingleSided = CapabilityChecker.GetModelBoolValue(selection, id("tribeStaffSingleSided"));
	UpdateStaffModel();
	UpdateStaffIcon();
}



// Detect when the planner is opened/hut page is loaded, and update the icon.

//-------------------------------------------

// Gets the hut icon resource path from the premade / user-generated hut
ResourceKey TRG_ChieftainManager::GetStaffIconResource(ResourceKey selection) const {
	// If the model is a premade prop, look for its thumbnail in the correct folder.
	if (selection.typeID == Names::prop) {
		selection.groupID = id("PlannerThumbnails");
	}
	return selection;
}

// TODO: make this work better
void TRG_ChieftainManager::UpdateStaffModel() {
	auto chieftain = GameNounManager.GetPlayerTribe()->GetLeaderCitizen();
	chieftain->PlayAnimation(0x02C39200); // get_tool
}

void TRG_ChieftainManager::UpdateStaffIcon() {
	ResourceKey staffResource;

	// If there is no resource, it should default back to staff image of the assigned diet type.
	if (mStaffRes == ResourceKey()) {
		int diet = GetChiefDietValue(GameNounManager.GetPlayerTribe()->GetLeaderCitizen(),false);
		switch (diet) {
			case 0:
				staffResource = ResourceKey(id("TRG_Staff_Carn_01"), prop, id("PaletteItems"));
				break;
			case 1:
				staffResource = ResourceKey(id("TRG_Staff_Herb_01"), prop, id("PaletteItems"));
				break;
			case 2:
				staffResource = ResourceKey(id("TRG_Staff_Omni_01"), prop, id("PaletteItems"));
				break;
			case 3:
				staffResource = ResourceKey(id("TRG_Staff_Mouth_01"), prop, id("PaletteItems"));
				break;
		}
	}
	else {
		staffResource = mStaffRes;
	}
	if (staffResource == ResourceKey()) { return; }
	
	auto window = WindowManager.GetMainWindow();
	auto imagewindow = window->FindWindowByID(0x7D08CAFF);

	if (imagewindow) {
		auto icon = GetStaffIconResource(staffResource);
		if (icon == ResourceKey()) { return; }
		object_cast<IImageDrawable>(imagewindow->GetDrawable())->SetImageForWindow(imagewindow, icon);
	}

}

int TRG_ChieftainManager::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool TRG_ChieftainManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsTribeGame()) { return false; }

	if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();
		if (message.source->GetParent()->GetControlID() != 0x04A100A1) { return false; }

		if (id >= 0x04AAFF6B && id <= 0x04AAFF6D) {
			OpenStaffShopper();
		}

	}
	return false;
}


Simulator::Attribute TRG_ChieftainManager::ATTRIBUTES[] = {
	// Add more attributes here
	SimAttribute(TRG_ChieftainManager,mStaffRes,1),
	SimAttribute(TRG_ChieftainManager,mbStaffSingleSided,2),
	// This one must always be at the end
	Simulator::Attribute()
};


// For internal use, do not modify.
int TRG_ChieftainManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int TRG_ChieftainManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* TRG_ChieftainManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(TRG_ChieftainManager);
	return nullptr;
}

void TRG_ChieftainManager::Initialize() {
}

void TRG_ChieftainManager::Dispose() {
}

const char* TRG_ChieftainManager::GetName() const {
	return "Valla_CreatureOverhaul::TRG_ChieftainManager";
}

bool TRG_ChieftainManager::Write(Simulator::ISerializerStream* stream)
{
	SporeDebugPrint("TRG_ChieftainManager is writing...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool TRG_ChieftainManager::Read(Simulator::ISerializerStream* stream)
{
	if (mStaffRes.instanceID != 0x0) {
		mStaffRes = ResourceKey();
	}
	mbStaffSingleSided = false;
	SporeDebugPrint("TRG_ChieftainManager is reading...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
}

void TRG_ChieftainManager::Update(int deltaTime, int deltaGameTime) {
	
}