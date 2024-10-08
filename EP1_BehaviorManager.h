#pragma once

#include <Spore\BasicIncludes.h>

#define EP1_BehaviorManagerPtr intrusive_ptr<EP1_BehaviorManager>

///
/// In your dllmain Initialize method, add the system like this:
/// ModAPI::AddSimulatorStrategy(new EP1_BehaviorManager(), EP1_BehaviorManager::NOUN_ID);
///

using namespace Simulator;
using namespace UTFWin;

class EP1_BehaviorManager
	: public Simulator::cStrategy
	, public IWinProc
{
public:
	static const uint32_t TYPE = id("Valla_CreatureOverhaul::EP1_BehaviorManager");
	static const uint32_t NOUN_ID = TYPE;

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

	void ApplyVehicleBehaviorUIs();
	void HideVehicleWeaponButton();
	void ClickedVehicleWeaponButton();

	//-------------------------------------------------------
	static Simulator::Attribute ATTRIBUTES[];
	int GetEventFlags() const override;
	/// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

private:
	UILayout newUIlayout;

};