#include "stdafx.h"
#include "TRG_CreaturePickup.h"
#include <Spore\UTFWin\Cursors.h>

TRG_CreaturePickup::TRG_CreaturePickup()
{
	App::AddUpdateFunction(this);
	CursorManager.Load(0x03C32077, u"cursor-grab_close");
}


TRG_CreaturePickup::~TRG_CreaturePickup()
{
}


//-----------------------------------------------------------------------------------------------

cTribePtr TRG_CreaturePickup::GetPlayerTribe() {
	auto tribedata = Simulator::GetDataByCast<Simulator::cTribe>();
	if (tribedata.size() < 1) { return nullptr; }
	return tribedata[0];
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
	held_member->PlayAnimation(0x074D1B28); // "EP1_lifted_CrTr_loop"

	// TODO: figure out what the grabbing closed cursor is
	//CursorManager.SetActiveCursor(BasicCursorIDs::Cursors::Link);

	held_member->mbPickable = false;
	held_member->mbIsInvalid = true;
}

void TRG_CreaturePickup::Drop() {
	if (!held_member) { return; }

	possible_member = nullptr;
	held_member->SetIsSelected(false);
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

// This method returns a combinations of values in UTFWin::EventFlags.
// The combination determines what types of events (messages) this window procedure receives.
// By default, it receives mouse/keyboard input and advanced messages.
int TRG_CreaturePickup::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

// The method that receives the message.
bool TRG_CreaturePickup::HandleUIMessage(IWindow* window, const Message& message)
{
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
				possible_member->mSelectionGroup = 0;
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