#include "stdafx.h"
#include "cCreaturePickup.h"
#include "Common.h"
#include <Spore\UTFWin\Cursors.h>

cCreaturePickup::cCreaturePickup()
{
	sInstance = this;
	App::AddUpdateFunction(this);
	CursorManager.Load(cur_grab_closed, u"cursor-grab_close");

	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, SimulatorMessages::kMsgSwitchGameMode);
	MessageManager.AddListener(this, id("ResetTimescale"));
}


cCreaturePickup::~cCreaturePickup()
{
	sInstance = nullptr;
}

cCreaturePickup* cCreaturePickup::sInstance;

cCreaturePickup* cCreaturePickup::Get()
{
	return sInstance;
}

// You can extend this function to return any other types your class implements.
void* cCreaturePickup::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(cCreaturePickup);
	return nullptr;
}

//-----------------------------------------------------------------------------------------------

bool cCreaturePickup::IsValidGameMode() const {
	for (auto item : VALID_GAME_MODES) {
		if (item == GetGameModeID()) { return true; }
	}
	return false;
}

bool cCreaturePickup::CanPickUp(cSpatialObjectPtr target) const {
	// TODO: return false if the creature is an epic/very large
	return (target && !IsPlannerOpen());
}

// TODO: expand this to deselect selectables from other stages and pickup allowances.
// Causing crashes?
void cCreaturePickup::DeselectMembers() {
	/*
	cTribePtr tribe = GetPlayerTribe();
	if (tribe) {
		auto members = tribe->GetSelectableMembers();
		for (auto member : members) {
			if (member && member != mpHeldMember) {
				member->SetIsSelected(false);
			}
		}
	}*/
}

//-----------------------------------------------------------------------------------------------


void cCreaturePickup::Update()
{
	if (!IsValidGameMode()) { return; }

	if (IsCreatureHeld()) {
		CursorManager.SetActiveCursor(cur_grab_closed);
		auto held_combatant = object_cast<cCreatureBase>(mpHeldMember);
		if (held_combatant) {
			held_combatant->mDesiredSpeed = 0.0;
			held_combatant->SetTarget(nullptr);
			held_combatant->mLastAttacker = 0xffffffff;

			auto held_creature = object_cast<cCreatureBase>(mpHeldMember);
			if (held_creature) {
				// if the hold anim is not playing, restart it.
				uint32_t* anim = nullptr;
				held_combatant->mpAnimatedCreature->GetCurrentAnimation(anim);
				if (anim && anim[0] != 0x074D1B28) {
					held_combatant->PlayAnimation(0x074D1B28); // "EP1_lifted_CrTr_loop"
				}
			}
		}
	}

}

// Return the hovered spatial object if it is valid for pickup, otherwise return nullptr
// TODO: THIS!
cSpatialObjectPtr cCreaturePickup::GetPickupObject() const {
	auto hovered = GameViewManager.GetHoveredObject();
	auto object = object_cast<cLocomotiveObject>(hovered);
	if (!object) {
		object = CityMemberManager.GetHoveredCitizen().get();
		if (object) { return object; }
	}
	if (object) {
		return object;
	}
	return nullptr;
}

// Set the object that should be picked up on mouse drag
// TODO: THIS!
void cCreaturePickup::SetPossibleMember() {
	mpPossibleMember = nullptr;
	mpHeldMember = nullptr;
	auto object = GetPickupObject();
	if (object && CanPickUp(object)) {
		auto citizen = object_cast<cCreatureCitizen>(object);
		if (citizen) {
			mpPossibleMember = citizen;
			citizen->SetIsSelected(true);
			citizen->mSelectionGroup = 1;
		}
	}
}

void cCreaturePickup::Pickup(cSpatialObjectPtr target) {

	auto creature = object_cast<cCreatureBase>(target);
	if (creature && !creature->mbDead) {
		mpHeldMember = creature;
		mpPossibleMember = nullptr;

		DeselectMembers();
		creature->mbDead = true; // dead means they cannot be attacked and cannot perform most actions
		creature->PlayAnimation(0x074D1B28); // "EP1_lifted_CrTr_loop"
	}

	if (mpHeldMember) {
		mpHeldMember->mbPickable = false;
		mpHeldMember->mbIsInvalid = true;
	}
}

void cCreaturePickup::Drop() {
	if (!IsCreatureHeld()) { return; }

	mpPossibleMember = nullptr;
	mpHeldMember->mbPickable = true;
	mpHeldMember->mbIsInvalid = false;
	mpHeldMember->SetIsSelected(true);
	CursorManager.SetActiveCursor(BasicCursorIDs::Cursors::Default);

	auto held_combatant = object_cast<cCreatureBase>(mpHeldMember);
	if (held_combatant) {
		held_combatant->mbDead = false;

		auto held_creature = object_cast<cCreatureBase>(mpHeldMember);
		if (held_creature) {
			held_combatant->PlayAnimation(0x07C7AF06); // "scenario_sentient_edit_place"
			held_combatant->MoveTo(mpHeldMember->GetPosition()); // resume normal behaviors
		}
	}

	// at the end, clear reference.
	mpHeldMember = nullptr;
}

void cCreaturePickup::Moved() {
	if (!IsCreatureHeld()) { return; }
	auto pos = GameViewManager.GetWorldMousePosition(0, object_cast<cCombatant>(mpHeldMember));
	if (pos != Vector3(0.0f, 0.0f, 0.0f)) {
		mpHeldMember->Teleport(pos, mpHeldMember->GetOrientation());
	}
}

//-----------------------------------------------------------------------------------------------
// fake a click

void cCreaturePickup::UnclickMB1(const Message& refmessage) {
	auto window = WindowManager.GetMainWindow();

	Message message;
	message.source = window;  // Which window generated this event?
	message.eventType = kMsgMouseUp;  // What type of event is it?
	// Now we want to set the specific parameters (i.e. the position that was clicked,...).
	message.Mouse.mouseButton = kMouseButtonLeft;
	message.Mouse.mouseState = refmessage.Mouse.mouseState & ~kMouseLeftButtonDown;

	message.Mouse.mouseX = refmessage.Mouse.mouseX;
	message.Mouse.mouseY = refmessage.Mouse.mouseY;

	mbTempDenyPickup = true;
	// Now just send the message
	window->SendMsg(message);
	if (mpHeldMember) {
		Drop();
	}
}

//-----------------------------------------------------------------------------------------------

bool cCreaturePickup::HandleMessage(uint32_t messageID, void* msg)
{
	if (!IsValidGameMode()) {
		if (messageID == SimulatorMessages::kMsgSwitchGameMode && mpHeldMember) {
			mpHeldMember = nullptr;
		}
		return nullptr;
	}

	if (messageID == SimulatorMessages::kMsgSwitchGameMode || messageID == id("ResetTimescale") || messageID == id("DropCreature"))
	{
		Drop();
	}
	return false;
}

int cCreaturePickup::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

// The method that receives the message.
bool cCreaturePickup::HandleUIMessage(IWindow* window, const Message& message)
{
	if (mbTempDenyPickup) { mbTempDenyPickup = false; return false; }

	if (!IsValidGameMode()) { return false; }

	// mouse moved away from a member
	if (message.IsType(kMsgMouseMove) && !IsPlannerOpen()) {

		if (!mpPossibleMember && CursorManager.GetActiveCursor() == BasicCursorIDs::Cursors::GrabOpen) {
			CursorManager.SetActiveCursor(0x0);
		}
		else if (!IsCreatureHeld() && GetPickupObject()) {
			CursorManager.SetActiveCursor(BasicCursorIDs::Cursors::GrabOpen);
		}
	}

	// Drop creature
	if (message.IsType(kMsgMouseUp) && IsCreatureHeld()) {
		Drop();
		UnclickMB1(message);
		return false;
	}

	// Player has left clicked the mouse. Register possible member to be grabbed on drag.
	else if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseDown) && !IsCreatureHeld()) {
		if (IsPlannerOpen()) { return false; }
		SetPossibleMember();
		// Do this last
		if (mpPossibleMember) {
			UnclickMB1(message);
			return true;
		}
	}

	// Player has moved the mouse
	if (message.Mouse.IsLeftButton() && message.IsType(kMsgMouseMove)) {
		// already held
		if (mpHeldMember) {
			Moved();
			return true;
		}
		// pick up new member
		else if (!mpHeldMember && mpPossibleMember) {
			if (CanPickUp(mpPossibleMember)) {
				Pickup(mpPossibleMember);
				return true;
			}
			return false;
		}
	}

	// Suppress camera rotation while holding a member
	if (message.Mouse.IsMiddleButton() && message.IsType(kMsgMouseDown) && mpHeldMember) {
		return true;
	}

	// Return true if the message was handled
	return false;
}

//-----------------------------------------------------------------------------------------------
