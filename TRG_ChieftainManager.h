#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\App\IMessageListener.h>

#define ChieftainManager  (*TRG_ChieftainManager::Get())
#define TRG_ChieftainManagerPtr intrusive_ptr<TRG_ChieftainManager>

using namespace Simulator;

class TRG_ChieftainManager 
	: public Object
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("TRG_ChieftainManager");
	
	TRG_ChieftainManager();
	~TRG_ChieftainManager();
	static TRG_ChieftainManager* Get();

	// Add newest object int to end, remove when applied
	// 0 = carn  1 = herb  2 = omni
	int tribe_staff_id = -1;

	int last_tribe_count = 0;
	int tribe_count = 0;

	void Reset();

	cCreatureCitizen* TRG_ChieftainManager::GetNearestTribalLeader(Vector3 pos, const float within) const;
	cCreatureCitizen* GetAnimCreatureOwner(const AnimatedCreaturePtr& animcreature);
	bool IsCreatureTribeLeader(const cCreatureCitizenPtr& creature);
	int GetChiefDietValue(const cCreatureCitizenPtr chief);
	void AddChiefToQueue(const cCreatureCitizenPtr chief);
	int NextQueueItem();

	uint32_t GetStaffID(int diet, int attachment) const;

	bool HandleMessage(uint32_t messageID, void* msg) override;
	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

protected:
	static TRG_ChieftainManager* sInstance;
};
