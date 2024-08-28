#include "stdafx.h"
#include "TRG_ChieftainManager.h"
#include "CapabilityChecker.h"
#include <Spore\Simulator\SimulatorMessages.h>

TRG_ChieftainManager::TRG_ChieftainManager()
{
	sInstance = this;
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
			App::ConsolePrintF(" - Chieftain has grabbed or put away tool.");
		}
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

	auto tribes = Simulator::GetDataByCast<Simulator::cTribe>();
	for (auto tribe : tribes) {
		cCreatureCitizen* leader = tribe->GetLeaderCitizen();
		float dist = (leader->GetPosition() - pos).SquaredLength();
		if (dist < last_distance) {
			closest_leader = leader;
		}
	}
	return closest_leader;
}

bool TRG_ChieftainManager::IsCreatureTribeLeader(const cCreatureCitizenPtr& creature) {
	// compare citizen to each tribal leaders till we find a match
	auto tribes = Simulator::GetDataByCast<Simulator::cTribe>();
	for (auto tribe : tribes) {
		if (creature == tribe->GetLeaderCitizen()) {
			return true;
		}
	}
	return false;
}

int TRG_ChieftainManager::GetChiefDietValue(cCreatureCitizenPtr chief) {
	int value = 0;
	if (!IsTribeGame()) { return value; }
	last_tribe_count = tribe_count;
	auto tribes = Simulator::GetDataByCast<Simulator::cTribe>();
	tribe_count = tribes.size();
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
				chief = tribes[tribe_count - 1]->GetLeaderCitizen();
			}
		}
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
	int value = GetChiefDietValue(chief);
	//App::ConsolePrintF(" - Storing diet value %i",value);
	tribe_staff_id = value;
}

int TRG_ChieftainManager::NextQueueItem() {
	int value = tribe_staff_id;
	if (value == -1) {
		value = GetChiefDietValue(nullptr);
	}
	tribe_staff_id = -1;
	return value;
}

uint32_t TRG_ChieftainManager::GetStaffID(int diet, int attachment) const {
	if (attachment > -1) {
		//App::ConsolePrintF(" - Accessing Chieftain diet value:  %x", dietvalue);

		//TODO: make this work with the savegames, fix edge cases, fix crashes when re-entering simulator etc
		switch (attachment) {
		case 0:
			if (diet == 1) {
				return id("trg_chieftain_staff_herb");
			}
			else if (diet == 2) {
				return id("trg_chieftain_staff_omni");
			}
			else if (diet == 3) {
				return id("trg_chieftain_staff_mouth");
			}
			break;
		case 1:
			if (diet == 1) {
				return id("trg_chieftain_staff_herb_fish");
			}
			else if (diet == 2) {
				return id("trg_chieftain_staff_omni_fish");
			}
			else if (diet == 3) {
				return id("trg_chieftain_staff_mouth_fish");
			}
			break;
		case 2:
			if (diet == 1) {
				return id("trg_chieftain_staff_herb_seaweed");
			}
			else if (diet == 2) {
				return id("trg_chieftain_staff_omni_seaweed");
			}
			else if (diet == 3) {
				return id("trg_chieftain_staff_mouth_seaweed");
			}
			break;
		}
	}
	return 0x0;
}

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
