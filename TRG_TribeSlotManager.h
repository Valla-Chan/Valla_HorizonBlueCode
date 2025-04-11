#pragma once

#include <Spore\BasicIncludes.h>

#define TRG_TribeSlotManagerPtr intrusive_ptr<TRG_TribeSlotManager>

///
/// In your dllmain Initialize method, add the system like this:
/// ModAPI::AddSimulatorStrategy(new TRG_TribeSlotManager(), TRG_TribeSlotManager::NOUN_ID);
///

using namespace Simulator;

class TRG_TribeSlotManager
	: public Simulator::cStrategy
{
public:
	static const uint32_t TYPE = id("TRG_TribeSlotManager");
	static const uint32_t NOUN_ID = TYPE;

	int AddRef() override;
	int Release() override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;

	//-----------------------------------------------------

	enum SlotType {
		Tool,
		Decor,
		Any,
	};

	cCommunityLayout& GetTribeLayout() const;

	void StoreSlots();
	void RestoreSlots();
	void AddSlot(SlotType slotType, Vector3 pos);

	void ActivateSlotType(SlotType slotType);

	static Simulator::Attribute ATTRIBUTES[];

private:

	eastl::vector<Vector3> mStoredSlotPositions;

	SlotType mCurrentSlotType = Tool;

};