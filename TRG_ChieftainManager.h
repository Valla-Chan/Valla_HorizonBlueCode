#pragma once

#include <Spore\BasicIncludes.h>

#define ChieftainManager  (*TRG_ChieftainManager::Get())
#define TRG_ChieftainManagerPtr intrusive_ptr<TRG_ChieftainManager>

using namespace UTFWin;
using namespace Simulator;
using namespace TypeIDs;

class TRG_ChieftainManager 
	: public IWinProc
	, public DefaultRefCounted
	, public Simulator::cStrategy
	, public App::IMessageListener
	, public Sporepedia::IShopperListener
{
public:
	static const uint32_t TYPE = id("TRG_ChieftainManager");
	static const uint32_t NOUN_ID = TYPE;
	
	TRG_ChieftainManager();
	~TRG_ChieftainManager();
	static TRG_ChieftainManager* Get();

	// Store the staff resource
	ResourceKey mStaffRes = {};
	bool mbStaffSingleSided = false;

	// Add newest object int to end, remove when applied
	// 0 = carn  1 = herb  2 = omni 3 = mouth  4 = override
	int tribe_staff_id = -1;

	int last_tribe_count = 0;
	int tribe_count = 0;


	void Reset();

	cCreatureCitizen* TRG_ChieftainManager::GetNearestTribalLeader(Vector3 pos, const float within) const;
	cCreatureCitizen* GetAnimCreatureOwner(const AnimatedCreaturePtr& animcreature);
	bool IsCreatureTribeLeader(const cCreatureCitizenPtr& creature);
	int GetChiefDietValue(const cCreatureCitizenPtr chief, bool use_override = true);
	void AddChiefToQueue(const cCreatureCitizenPtr chief);
	int NextQueueItem();

	uint32_t GetStaffID(int diet, int attachment) const;
	uint32_t GetStaffID(const ResourceKey &staffres, int attachment) const;

	void OpenStaffShopper();
	void OnShopperAccept(const ResourceKey& selection) override;

	ResourceKey GetStaffIconResource(ResourceKey selection) const;
	Graphics::Texture* GetStaffIcon(int hutType) const;
	void UpdateStaffModel();
	void UpdateStaffIcon();

	bool HandleMessage(uint32_t messageID, void* msg) override;
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t typeID) const override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;

	static Simulator::Attribute ATTRIBUTES[];

protected:
	static TRG_ChieftainManager* sInstance;
};
