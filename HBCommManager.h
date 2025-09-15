#pragma once

#include "stdafx.h"
#include <Spore\BasicIncludes.h>
#include "Common.h"
#include <Spore\Simulator\SubSystem\CommManager.h>
// CVG
#include "CommsAnims.h"
#include "CityWallsManager.h"
#include "CityMemberManager.h"

enum HBCommAction {
	kCnvDiplomacy = id("kCnvDiplomacy"), // returns to the diplomacy screen
	kCnvMenu = id("kCnvMenu"), // returns to the civ menu

	kCnvComplimentText = id("kCnvCompliment"), // adds +5 score but doesnt cut to compliment preset dialogue. Can segue to another dialogue line
	kCnvInsultText = id("kCnvInsult"), // insults the nation but doesnt cut to compliment preset dialogue. Can segue to another dialogue line

	kCnvCityCompliment = id("kCnvCityCompliment"), // hated if you are at war
	// adds +5 score to these, and insults the other.
	// TODO: how to detect npc archetype?
	kCnvComplimentEcoMil = id("kCnvComplimentEcoMil"),
	kCnvComplimentRelMil = id("kCnvComplimentRelMil"),
	kCnvComplimentEcoRel = id("kCnvComplimentEcoRel"),

	kCnvTractsOfLand = id("kCnvTractsOfLand"), // hated by small nations
	kCnvSpaceSlugs = id("kCnvSpaceSlugs"), // hated by all except creatures with no hands or feet, and very friendly relig (as are we all) and econ (You are so funny! Hold on, we gotta tell that one to our other guys, haha!)

	kCnvHugeGift = id("kCnvHugeGift"), // gift of $8000

	///-------------
	/// Space Stage

	kCnvChangeBackground = id("kCnvChangeBackground"),
};

#define cHBCommManagerPtr intrusive_ptr<cHBCommManager>
#define HBCommManager (*cHBCommManager::Get())

class cHBCommManager
	: public DefaultRefCounted
	//, public IWinProc
	, public Simulator::cStrategy
	, public App::IMessageListener
	, public Sporepedia::IShopperListener
{

public:
	static const uint32_t TYPE = id("HBCommManager");
	static const uint32_t NOUN_ID = TYPE;

	cCommEvent* mpLastEvent = nullptr;
	Anim::AnimatedCreature* mpCommCreature = nullptr;
	cEmpire* mpCommEmpire = nullptr;
	uint32_t mPlayerID = 0x0;

	ResourceKey mPlayerCommBckg; // Can be a background image resource or a background creation resource (building)

	cHBCommManager();
	~cHBCommManager();

	ResourceKey GetCurrentCommEventSpeciesKey();
	bool IsCurrentCommEventPlayer();
	bool IsCurrentCommEventGrox();

	void OpenCommBckgShopper();
	void OnShopperAccept(const ResourceKey& selection) override;
	void ShowCommEventBckgReact();

	void ApplyCommCreatureColor();
	void ApplyCommBackground();
	void CommBackgroundBakeCallback();
	void ApplyCommBackgroundCreation();
	void AddCreatureToCommWindow(const ResourceKey& key, Vector3 position = Vector3(), Quaternion orientation = Quaternion());

	//---------------------------
	static cHBCommManager* Get();
	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t typeID) const;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	virtual bool WriteToXML(XmlSerializer*) override { return true; }
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;

	bool HandleMessage(uint32_t messageID, void* msg) override;
	//int GetEventFlags() const override;
	//bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

	static void AttachDetours();

	static Simulator::Attribute ATTRIBUTES[];

protected:
	static cHBCommManager* sInstance;

};


//----------------
// DETOUR HELPERS

// Detour the playanimation ID-picking func
static bool CVG_Convo_AnimOverride_detour(Anim::AnimatedCreature* obj, uint32_t& animID, int* pChoice) {

	if (!(IsCivGame() || IsSpaceGame())) { return false; }

	// todo: not working
	if (animID == 0x0564FB23) {
		animID = 0x0761e188;
		return true;
	}

	bool is_comms_creature = false;
	if (CommManager.IsCommScreenActive()) {
		for (auto anim : anims_comms_action) {
			if (animID == anim) {
				is_comms_creature = true; break;
			}
		}
		for (auto anim : anims_comms_idle) {
			if (animID == anim) {
				is_comms_creature = true; break;
			}
		}
	}
	// Change comms creature color based on their civ
	if (is_comms_creature) {
		HBCommManager.mpCommCreature = obj;
	}
	return false;
}

//---------
// DETOURS

// Space stage comms
member_detour(Comms_CreateSpaceCommEvent_detour, Simulator::cCommManager, void(uint32_t, PlanetID, uint32_t, uint32_t, cMission*, int, unsigned int)) {
	void detoured(uint32_t sourceEmpire, PlanetID planetKey, uint32_t fileID, uint32_t dialogID, cMission* pMission = nullptr, int priority = 0, unsigned int duration = 0)
	{
		HBCommManager.mpCommEmpire = StarManager.GetEmpire(sourceEmpire);
		return original_function(this, sourceEmpire, planetKey, fileID, dialogID, pMission, priority, duration);
	}
};

// Open comms
member_detour(Comms_ShowCommEvent_detour, Simulator::cCommManager, void(cCommEvent*)) {

	void Comms_ShowCommEvent_detour::UpdateComm() {
		UpdateCommBackground();
		UpdateTradeButton();
		UpdateDiplomacyButton();

		// Exclude player and grox from coloring
		if (!HBCommManager.IsCurrentCommEventPlayer() && !HBCommManager.IsCurrentCommEventGrox()) {
			HBCommManager.ApplyCommCreatureColor();
		}
	}

	void Comms_ShowCommEvent_detour::UpdateCommBackground() {
		// Only do this for the player.
		if (HBCommManager.IsCurrentCommEventPlayer()) {
			HBCommManager.ApplyCommBackground();
		}
	}

	void Comms_ShowCommEvent_detour::UpdateTradeButton() {
		// Allow grox trade when allied
		if (IsSpaceGame() && HBCommManager.IsCurrentCommEventGrox()) {
			auto btnw = CommManager.GetCommWindow(kBtnTrade);
			if (RelationshipManager.IsAllied(StarManager.GetGrobEmpireID(), GameNounManager.GetPlayer()->GetPoliticalID())) {
				if (btnw) btnw->SetEnabled(true);
			}
			else {
				if (btnw) btnw->SetEnabled(false);
			}
		}
	}

	void Comms_ShowCommEvent_detour::UpdateDiplomacyButton() {
		// Change Diplomacy button to "Conversation"
		if (IsSpaceGame()) {
			auto btnw = CommManager.GetCommWindow(kBtnDiplomacy);

			if (btnw && btnw->IsVisible()) {

				LocalizedString locale = LocalizedString(id("ui_space"), 0x0627EB7D);
				if (HBCommManager.IsCurrentCommEventPlayer()) {
					locale = LocalizedString(id("space_convo_hb"), 0x0000000A);
					btnw->SetEnabled(true);
				}
				for (auto E : btnw->children()) {
					if (E->GetArea().GetWidth() == 0) {
						E->SetCaption(locale.GetText());
					}
				}
			}
		}
	}

	
	void Comms_ShowCommEvent_detour::ShowLastCommEvent() {
		original_function(this, HBCommManager.mpLastEvent);
		HBCommManager.mpCommEmpire = StarManager.GetEmpire(HBCommManager.mpLastEvent->mSource);
		this->UpdateComm();
		// Last
		HBCommManager.mpLastEvent = nullptr;
	}

	void detoured(cCommEvent* pEvent) {
		// Fix game time scale by sending msg and delaying comms
		if (!IsCommScreenActive() && (IsSpaceGame() || IsCivGame())) {
			MessageManager.MessageSend(id("ResetTimescale"), nullptr);
			HBCommManager.mpLastEvent = pEvent;


			// Fill in missing city with first city (if communicating with a city)
			if (!pEvent->mpTargetCity && pEvent->mpSourceCivilization && pEvent->mpSourceCivilization->mCities.size() > 0) {
				pEvent->mpTargetCity = pEvent->mpSourceCivilization->mCities[0].get();
			}

			App::ScheduleTask(this, &Comms_ShowCommEvent_detour::ShowLastCommEvent, 0.001f);
			return;
		}
		//------------------------------------------------------
		// TODO: eventually make a UPE-like/effect-overrider-like system for staticly replacing comm events?

		if (pEvent->mFileID == 0xC8FBF7D7 && pEvent->mDialogID == id("diplomacy") && HBCommManager.IsCurrentCommEventPlayer()) {
			pEvent->mFileID = id("space_diplomacy_hb");
			pEvent->mDialogID = id("player_diplomacy");
		}

		original_function(this, pEvent);
		this->UpdateComm();
	}
};


// Comms actions for SPACE
// This is for choosing an "action" as a response. like `action exit`
member_detour(Comms_HandleSpaceCommAction_detour, LocalizedString, void(const CnvAction&, uint32_t, PlanetID, cMission*)) {
	void detoured(const CnvAction& action, uint32_t sourceEmpireID, PlanetID planetKey, cMission* pMission) {
		CnvAction newaction = action;
		bool playkeyconvo = true; // set to false if using the action.key for something other than the follow-up conversation

		uint32_t player_id = 0x0;
		if (GameNounManager.GetPlayerCivilization()) player_id = GameNounManager.GetPlayerCivilization()->mPoliticalID;

		switch (action.actionID) {
			// Open menus
			case kCnvDiplomacy:
				newaction.key = { id("diplomacy"), Names::cnv, 0xC8FBF7D7 };
				break;
			case kCnvMenu:
				newaction.key = { id("default"), Names::cnv, 0xC8FBF7D7 };
				break;

			case kCnvChangeBackground:
				HBCommManager.OpenCommBckgShopper();
				//newaction.key = { id("default"), Names::cnv, 0xC8FBF7D7 };
				newaction.actionID = kCnvCommExit;
				break;
		}

		// If playkeyconvo and a key is specified, play it as next conversation
		if (action.actionID != kCnvCommEvent && playkeyconvo &&
			(newaction.key.typeID == Names::cnv && newaction.key.groupID != 0x0 && newaction.key.instanceID != 0x0)) {
			newaction.actionID = kCnvCommEvent;
		}
		// if key is "exit", play the exit command.
		if (action.actionID != kCnvCommExit && newaction.key.instanceID == kCnvCommExit) {
			newaction.key = {};
			newaction.actionID = kCnvCommExit;
		}

		original_function(this, newaction, sourceEmpireID, planetKey, pMission);

	}
};

// Comms actions for CIV
// This is for choosing an "action" as a response. like `action exit`
member_detour(Comms_HandleCivCommAction_detour, LocalizedString, void(const CnvAction&, cCivilization*, cCity*, cCity*)) {
	void detoured(const CnvAction& action, cCivilization* pSourceCiv, cCity* pSourceCity, cCity* pTargetCity) {

		CnvAction newaction = action;
		bool playkeyconvo = true; // set to false if using the action.key for something other than the follow-up conversation

		uint32_t player_id = 0x0;
		if (GameNounManager.GetPlayerCivilization()) player_id = GameNounManager.GetPlayerCivilization()->mPoliticalID;

		switch (action.actionID) {
			// Open menus
			case kCnvDiplomacy:
				newaction.key = { id("npc_diplomacy"), Names::cnv, id("civ_convo_hb_diplomacy") };
				break;
			case kCnvMenu:
				newaction.key = { id("player_contact_npc"), Names::cnv, id("civ_main_menu") };
				break;

			// Generic compliment + text
			case kCnvComplimentText:
				RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventCompliment, 1.0f);
				break;
			case kCnvInsultText:
				RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventInsult, 1.0f);
				break;

			// Modded base game insults/compliments
			case kCnvTractsOfLand:
				// check empire city/commodity size
				{
					int playercities = GameNounManager.GetPlayerCivilization()->mCities.size();
					int npccities = pSourceCiv->mCities.size();
					int playerspice = 0;
					int npcspice = 0;

					for (auto mine : GetData<cCommodityNode>()) {
						if (mine->mPoliticalID == player_id) playerspice++;
						else if (mine->mPoliticalID == pSourceCiv->mPoliticalID) npcspice++;
					}
					if ((playercities + playerspice / 2 - 2) > (npccities + npcspice)) {
						RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventInsult, 1.0f);
						newaction.key = { id("npc_respond_tracksofland_neg"), Names::cnv, id("civ_convo_hb_diplomacy") };
					}
					else {
						RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventCompliment, 1.0f);
						newaction.key = { id("npc_respond_tracksofland_pos"), Names::cnv, id("civ_convo_hb_diplomacy") };
					}
				}
				break;
			case kCnvSpaceSlugs:
				// Todo: check species for hands/feet.
				//CapabilityChecker::GetCapabilityLevelFromResource(pSourceCiv->creat, id("Creature"), 1)
				RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventInsult, 1.0f);
				newaction.key = { id("npc_insulted"), Names::cnv, id("civ_main_menu") };
				//newaction.key = { id("npc_respond_spaceslugs_pos"), Names::cnv, id("civ_convo_hb_diplomacy") };
				break;

			// New insults/compliments
			case kCnvCityCompliment:
				
				/// Negative
				// being attacked
				if (pTargetCity->mCapturePercent > 0 && pTargetCity->mCaptureID == player_id) {
					RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventInsult, 1.0f);
					newaction.key.groupID = id("civ_convo_hb_diplomacy");
					newaction.key.instanceID = id("npc_respond_city_negative_capture");
				}
				// being converted
				else if (pTargetCity->mConvertPercent > 0 && pTargetCity->mConvertID == player_id) {
					RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventInsult, 1.0f);
					newaction.key.groupID = id("civ_convo_hb_diplomacy");
					newaction.key.instanceID = id("npc_respond_city_negative_convert");
				}
				// not being attacked but is at war
				else if (RelationshipManager.IsAtWar(pSourceCiv->mPoliticalID, player_id)) {
					newaction.key.groupID = id("civ_convo_hb_diplomacy");
					newaction.key.instanceID = id("npc_respond_city_negative_war");
				}
				/// Positive
				else {
					RelationshipManager.ApplyRelationship(pSourceCiv->mPoliticalID, player_id, kRelationshipEventCompliment, 1.0f);
					newaction.key.groupID = id("civ_convo_hb_diplomacy");
					if (pTargetCity->mbSmallCity) {
						newaction.key.instanceID = id("npc_respond_city_positive_sml");
					}
					else {
						newaction.key.instanceID = id("npc_respond_city_positive_lrg");
					}
				}
				break;

			// New bribes
			// TODO: broken, needs fixing of values
			case kCnvHugeGift:
				// Play 4000 gift event twice
				newaction.actionID = kCnvLargeGift;
				original_function(this, newaction, pSourceCiv, pSourceCity, pTargetCity);
				original_function(this, newaction, pSourceCiv, pSourceCity, pTargetCity);
				return;
		}

		// If playkeyconvo and a key is specified, play it as next conversation
		if (action.actionID != kCnvCommEvent && playkeyconvo &&
			(newaction.key.typeID == Names::cnv && newaction.key.groupID != 0x0 && newaction.key.instanceID != 0x0 )) {
			newaction.actionID = kCnvCommEvent;
		}
		// if key is "exit", play the exit command.
		if (action.actionID != kCnvCommExit && newaction.key.instanceID == kCnvCommExit) {
			newaction.key = {};
			newaction.actionID = kCnvCommExit;
		}

		original_function(this, newaction, pSourceCiv, pSourceCity, pTargetCity);
	}
};
