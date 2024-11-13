#include "stdafx.h"
#include "CRG_AttackBasic.h"
#include <Spore\Simulator.h>

CRG_AttackBasic::CRG_AttackBasic()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, id("CRG_AttackGeneric")); // listen for anim event
	MessageManager.AddListener(this, id("CRG_AttackGeneric_Done")); // listen for anim event
}


CRG_AttackBasic::~CRG_AttackBasic()
{
}

// For internal use, do not modify.
int CRG_AttackBasic::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CRG_AttackBasic::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* CRG_AttackBasic::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(CRG_AttackBasic);
	return nullptr;
}

// This method returns a combinations of values in UTFWin::EventFlags.
// The combination determines what types of events (messages) this window procedure receives.
// By default, it receives mouse/keyboard input and advanced messages.
int CRG_AttackBasic::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}




// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool CRG_AttackBasic::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!(Simulator::IsCreatureGame() || (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode))
		|| GameTimeManager.IsPaused()) { return false; }

	auto avatar = GameNounManager.GetAvatar();
	if (!avatar) { return false; }

	// Did the user press the F key? 
	if ((!attacking || true) && message.IsType(kMsgKeyDown) && message.Key.vkey == 'F') {
		attacking = true;
		avatar->PlayAnimation(0xAFF68933);
		//avatar->PlayAnimation(0xDE283096); // block
		return true; // Message handled
	}
	return false;
}

bool CRG_AttackBasic::HandleMessage(uint32_t messageID, void* msg)
{
	if (!(Simulator::IsCreatureGame() || (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode))
		|| GameTimeManager.IsPaused()) {
		return false;
	}



	if (messageID == id("CRG_AttackGeneric"))
	{
		AffectNearbyCreatures();
		AffectClosestInteractable();
		return true;
	}
	else if (messageID == id("CRG_AttackGeneric_Done")) {
		attacking = false;
		return true;
	}
	return false;
}

void CRG_AttackBasic::AffectClosestInteractable() {
	auto avatar = GameNounManager.GetAvatar();
	if (!avatar) { return; }

	float last_distance = attackrange;
	cInteractiveOrnamentPtr object_nearest = nullptr;


	// loop through all ingame interactables
	auto interactables = GetData<cInteractiveOrnament>();
	for (auto object : interactables) {
		float dist = abs((avatar->GetPosition() - object->GetPosition()).Length());
		if (dist < last_distance) {
			object_nearest = object.get();
			last_distance = dist;
		}
	}
	if (object_nearest) {
		object_nearest->SetScale(0.3f);
		//App::CreatureModeStrategies::Interact action = { GameNounManager.GetAvatar(), object_cast<Simulator::cGameData>(object_nearest), 0};
		//CreatureModeStrategy.ExecuteAction(action.ID, &action);
	}
}

void CRG_AttackBasic::AffectNearbyCreatures() {
	float within = attackrange;

	auto avatar = GameNounManager.GetAvatar();
	if (!avatar) { return; }
	auto creatures = Simulator::GetData<Simulator::cCreatureAnimal>();

	for (auto creature : creatures) {
		if (creature && creature.get() != avatar && abs((avatar->GetPosition() - creature->GetPosition()).Length()) <= within) {
			AffectCreature(creature);
		}
	}
}

void CRG_AttackBasic::AffectCreature(cCreatureAnimalPtr creature) {
	auto avatar = GameNounManager.GetAvatar();
	if (!avatar || avatar->mbDead || avatar->mAge < 1) { return; }

	creature->SetCreatureTarget(avatar, true, 1);
	//creature->mpLastAttacker = avatar;
	creature->SetTarget(avatar);
	creature->mpCombatantTarget = avatar;

	creature->PlayAnimation(0x02481E14); //com_stun
	//creature->PlayAnimation(0x92314082); //gen_startled
	creature->SetHealthPoints(max(creature->mHealthPoints - 1.0f, 0.0f));

	if (avatar->mbIsDiseased) {
		creature->mbIsDiseased = true;
	}
}