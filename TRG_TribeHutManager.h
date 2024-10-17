#pragma once

#include <Spore\BasicIncludes.h>

#define TRG_TribeHutManagerPtr intrusive_ptr<TRG_TribeHutManager>

using namespace UTFWin;
using namespace Simulator;
using namespace TypeIDs;

class TRG_TribeHutManager 
	: public IWinProc
	, public DefaultRefCounted
	, public Simulator::cStrategy
	, public Sporepedia::IShopperListener
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("TRG_TribeHutManager");
	static const uint32_t NOUN_ID = TYPE;
	
	TRG_TribeHutManager();
	~TRG_TribeHutManager();

	//---------------------------------------------------------

	enum HutType {
		Main = 0,
		Home = 1,
	};
	enum HomeRange {
		// Allocate 10 slots for tribal homes
		HomeStart = 12,
		HomeEnd = 22,
		//
	};

	/// Store tribe names
	bool has_pulled_tribenames = false;
	vector<string16> mTribeNames = { string16(u"Central Tribe") };

	/// Determines which hut slot to apply the sporepedia visuals to
	int mHutType = -1;

	// Store the Sporepedia entries
	ResourceKey mHutResMain = {};
	ResourceKey mHutResHome = {};

	void UpdateNPCTribeNames();
	void SetTribeName(cTribePtr tribe);
	void UpdateStoredTribeNames();
	void UpdateTribeNamesFromStored();

	void OpenHutShopper();
	void OnShopperAccept(const ResourceKey& selection) override;
	void OnModeEntered(uint32_t previousModeID, uint32_t newModeID) override;

	//---------------------------------------------------------
	
	ResourceKey GetHutIconResource(ResourceKey selection) const;
	Graphics::Texture* GetHutIcon(int hutType) const;
	void UpdateHutIcon(int hutType);
	void UpdateHutModels(int hutType);

	//---------------------------------------------------------
	
	bool HandleMessage(uint32_t messageID, void* msg) override;
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;


	//---------------------------------------------------------

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

};
