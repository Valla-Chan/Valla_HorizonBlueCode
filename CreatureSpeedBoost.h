#pragma once

#include <Spore\BasicIncludes.h>

//#define CreatureSpeedBoostManager  (*CreatureSpeedBoost::Get())
#define CreatureSpeedBoostPtr intrusive_ptr<CreatureSpeedBoost>

using namespace Simulator;
class CreatureSpeedBoost
	: public Object
	, public DefaultRefCounted
	, public Simulator::cStrategy
	//, public app::imessagelistener
{
public:
	static const uint32_t TYPE = id("Valla_HorizonBlue::CreatureSpeedBoost");
	static const uint32_t NOUN_ID = TYPE;

	CreatureSpeedBoost();
	~CreatureSpeedBoost();

	static CreatureSpeedBoost* Get();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;

	//---------------------------------------------------
	
	// creature ID and speed cap
	eastl::hash_map<uint32_t, float> mpBoostedCreatures;
	//const uint32_t MsgApplySpeedBoost = id("MsgApplySpeedBoost");
	//const uint32_t MsgRemoveSpeedBoost = id("MsgApplySpeedBoost");

	void ApplySpeedBoost(cCreatureBasePtr creature, float newSpeed);
	void RemoveSpeedBoost(cCreatureBasePtr creature);
	void RemoveAll();

	//bool HandleMessage(uint32_t messageID, void* msg) override;
	//---------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];

private:
	//
	// You can add members here
	//
protected:
	static CreatureSpeedBoost* sInstance;
};