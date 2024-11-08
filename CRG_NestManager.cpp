#include "stdafx.h"
#include "CRG_NestManager.h"

CRG_NestManager::CRG_NestManager()
{
	MessageManager.AddListener(this, kMsgCombatantKilled);
	App::AddUpdateFunction(this);
}


CRG_NestManager::~CRG_NestManager()
{
}

// For internal use, do not modify.
int CRG_NestManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CRG_NestManager::Release()
{
	return DefaultRefCounted::Release();
}


void CRG_NestManager::Update() {
	if (IsCreatureGame()) {
		auto hovered = GameViewManager.GetHoveredObject();
		// if hovered over a nest, show a rollover spui
		auto nest = object_cast<cNest>(hovered);
		if (nest && nest->IsPlayerOwned()) {
			UI::SimulatorRollover::ShowRollover(nest);
			auto window = WindowManager.GetMainWindow();
			auto nameBackground = window->FindWindowByID(0x933103EB);

			/*
			if (nameBackground) {
				ResourceObjectPtr res;
				if (GameNounManager.GetAvatar() && ResourceManager.GetResource(GameNounManager.GetAvatar()->mSpeciesKey, &res)) {
					auto resource = object_cast<Editors::cEditorResource>(res);
					auto colorRGB = resource->mProperties.mSkinColor1;
					nameBackground->SetShadeColor(colorRGB.ToIntColor());
				}
			}*/
			
		}
	}
}

// if the last nest was extincted, make it belong to the player.
void CRG_NestManager::CheckNestForExtinction() {
	// TURN THIS OFF FOR NOW
	return;

	auto avatar = GameNounManager.GetAvatar();
	if (lastNest->mpHerd->mbExtinction && avatar) {
		//lastNest->mpHerd = avatar->mHerd;
		lastNest->mpHerd->mpSpeciesProfile = avatar->mpSpeciesProfile;
	}
}

// The method that receives the message. The first thing you should do is checking what ID sent this message...
bool CRG_NestManager::HandleMessage(uint32_t messageID, void* message)
{

	if (!IsCreatureGame()) { return false; }

	if (messageID == kMsgCombatantKilled) {
		auto killMsg = (CombatantKilledMessage*)message; // cast to this message type
		if (killMsg) {
			auto avatar = GameNounManager.GetAvatar();

			// if killed by player
			if (killMsg->GetAttacker() == avatar) {
				auto nests = GetDataByCast<cNest>();
				auto killedCreature = object_cast<cCreatureAnimal>(killMsg->GetCombatant());
				if (!killedCreature) { return false; }

				// search nests for killed creature
				for (auto nest : nests) {
					// if found a matching nest, check if it is extincted and if so, change the ownership to the player species
					if (nest->mpHerd && nest->mpHerd->mpSpeciesProfile == killedCreature->mpSpeciesProfile) {
						lastNest = nest;
						Simulator::ScheduleTask(this, &CRG_NestManager::CheckNestForExtinction, 0.1f);
						return false;
					}
				}
			}
		}
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return false;
}
