#include "stdafx.h"
#include "TRG_CreaturePickup.h"
#include <Spore\UTFWin\Cursors.h>

TRG_CreaturePickup::TRG_CreaturePickup()
{
	App::AddUpdateFunction(this);
	CursorManager.Load(0x03C32077, u"cursor-grab_close");

	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, SimulatorMessages::kMsgSwitchGameMode);
	MessageManager.AddListener(this, id("CinematicBegin"));
}


TRG_CreaturePickup::~TRG_CreaturePickup()
{
}


//-----------------------------------------------------------------------------------------------

cTribePtr TRG_CreaturePickup::GetPlayerTribe() {
	return GameNounManager.mpPlayerTribe;
}


void TRG_CreaturePickup::DeselectMembers() {
	cTribePtr tribe = GetPlayerTribe();
	if (tribe) {
		auto members = tribe->GetSelectableMembers();
		for (auto member : members) {
			if (member && member != held_member) {
				member->SetIsSelected(false);
			}
		}
	}
}

void TRG_CreaturePickup::Update()
{
	if (!IsTribeGame()) { return; }

	if (held_member) {
		held_member->mDesiredSpeed = 0.0;
		held_member->SetTarget(nullptr);
		held_member->mLastAttacker = 0xffffffff;

		CursorManager.SetActiveCursor(0x03C32077);

		// if the hold anim is not playing, restart it.
		uint32_t* anim = nullptr;
		held_member->mpAnimatedCreature->GetCurrentAnimation(anim);
		if (anim && anim[0] != 0x074D1B28) {
			held_member->PlayAnimation(0x074D1B28); // "EP1_lifted_CrTr_loop"
		}
	}
	// Set cursor
	else {
		auto tribe = GetPlayerTribe();
		if (!tribe) { return; }
		auto members = tribe->GetSelectableMembers();
		for (auto member : members) {
			if (member && member->IsRolledOver()) {
				CursorManager.SetActiveCursor(BasicCursorIDs::Cursors::GrabOpen);
			}
		}
	}
	
}

void TRG_CreaturePickup::Pickup(cCreatureCitizenPtr creature) {
	if (!creature || creature->mbDead) { return; }
	held_member = creature;
	possible_member = nullptr;

	DeselectMembers();
	held_member->mbDead = true; // dead means they cannot be attacked and cannot perform most actions
	//held_member->PlayAnimation(0x03A74A0B); // "mot_item_droppanic"
	held_member->PlayAnimation(0x074D1B28); // "EP1_lifted_CrTr_loop"

	held_member->mbPickable = false;
	held_member->mbIsInvalid = true;
}

void TRG_CreaturePickup::Drop() {
	if (!held_member) { return; }

	possible_member = nullptr;
	held_member->mbDead = false;
	held_member->mbPickable = true;
	held_member->mbIsInvalid = false;
	held_member->SetIsSelected(true);

	held_member->PlayAnimation(0x07C7AF06); // "scenario_sentient_edit_place"
	held_member->MoveTo(held_member->GetPosition()); // resume normal behaviors

	CursorManager.SetActiveCursor(BasicCursorIDs::Cursors::Default);

	// at the end, clear reference.
	held_member = nullptr;
}

void TRG_CreaturePickup::Moved() {
	if (!held_member) { return; }
	auto pos = GameViewManager.GetWorldMousePosition(0, object_cast<cCombatant>(held_member));
	//SporeDebugPrint("location: %f , %f , %f", pos.x, pos.y, pos.z);
	if (pos != Vector3(0.0f, 0.0f, 0.0f)) {
		held_member->Teleport(pos, held_member->GetOrientation());
	}
}

//-----------------------------------------------------------------------------------------------
// fake a click on the creature's posse icon
void TRG_CreaturePickup::ClickPosseUI(cCreatureBasePtr creature) {
	// todo: not working.
	auto window = WindowManager.GetMainWindow();
	auto posseItem = window->FindWindowByID(0x03D98299);

	Message message;
	message.source = posseItem;  // Which window generated this event?
	message.eventType = kMsgMouseDown;  // What type of event is it?
	// Now we want to set the specific parameters (i.e. the position that was clicked,...).
	message.Mouse.mouseState = kMouseLeftButtonDown;
	message.Mouse.mouseX = posseItem->GetArea().GetWidth() / 2.0f;
	message.Mouse.mouseY = posseItem->GetArea().GetHeight() / 2.0f;
	

	temp_deny_pickup = true;
	// Now just send the message
	window->SendMsg(message);
}

void TRG_CreaturePickup::UnclickMB1() {
	auto window = WindowManager.GetMainWindow();

	Message message;
	message.source = window;  // Which window generated this event?
	message.eventType = kMsgMouseDown;  // What type of event is it?
	// Now we want to set the specific parameters (i.e. the position that was clicked,...).
	message.Mouse.mouseState = kMouseLeftButtonDown;

	

	//message.Mouse.mouseX = Cursor
	//message.Mouse.mouseY = posseItem->GetArea().GetHeight() / 2.0f;


	temp_deny_pickup = true;
	// Now just send the message
	window->SendMsg(message);
}

//-----------------------------------------------------------------------------------------------

bool TRG_CreaturePickup::HandleMessage(uint32_t messageID, void* msg)
{
	if (!IsTribeGame()) { return nullptr; }

	if (messageID == SimulatorMessages::kMsgSwitchGameMode || messageID == id("CinematicBegin"))
	{
		Drop();
	}
	return false;
}

int TRG_CreaturePickup::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

// The method that receives the message.
bool TRG_CreaturePickup::HandleUIMessage(IWindow* window, const Message& message)
{
	if (temp_deny_pickup) { temp_deny_pickup = false; return false; }

	if (!IsTribeGame()) { return false; }

	// Drop creature
	if (message.IsType(kMsgMouseUp) && held_member) {
		Drop();
		possible_member = nullptr;
	}

	// Player has left clicked the mouse
	else if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseDown) && !held_member) {
		// Loop through all tribal citizens in avatar tribe
		cTribePtr tribe = GetPlayerTribe();
		if (!tribe) { return false; }
		auto members = tribe->GetSelectableMembers();

		for (auto member : members) {
			if (member && member->IsRolledOver()) {
				// store this creature
				possible_member = object_cast<cCreatureCitizen>(member);
				possible_member->SetIsSelected(true);
				possible_member->mSelectionGroup = 1;

				// TODO: Spoof an unclick after this, so that the box select is not activated.
				ClickPosseUI(nullptr);

				return true;
			}
		}
		
	}
	// Player has moved the mouse
	if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseMove)) {
		// already held
		if (held_member) {
			Moved();
			return true;
		}
		// pick up new member
		else if (!held_member && possible_member) {
			Pickup(possible_member);
			return true;
		}
		
	}

	// Suppress camera rotation while holding a member
	if (message.Mouse.IsMiddleButton() && message.IsType(kMsgMouseDown) && held_member) {
		return true;
	}
	
	// Return true if the message was handled
	return false;
}

//-----------------------------------------------------------------------------------------------




// For internal use, do not modify.
int TRG_CreaturePickup::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int TRG_CreaturePickup::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* TRG_CreaturePickup::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(TRG_CreaturePickup);
	return nullptr;
}