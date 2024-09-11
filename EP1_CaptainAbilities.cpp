#include "stdafx.h"
#include "EP1_CaptainAbilities.h"

EP1_CaptainAbilities::EP1_CaptainAbilities()
{
}


EP1_CaptainAbilities::~EP1_CaptainAbilities()
{
}


int EP1_CaptainAbilities::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

//-----------------------------------------------------------------------------------------------



bool EP1_CaptainAbilities::CanDoubleJump(cCreatureAnimalPtr creature, int max_jumps) const {
	if (jumpcount >= max_jumps) { return false; }
	// creature is in midair, and is playing the default jump anim
	if (creature && !creature->mbSupported) {
		if (can_jumpburst) { //"gen_jump_loop"
			return true;
		}
	}
	return false;
}

// jump again in midair
bool EP1_CaptainAbilities::DoubleJump(float fwd_speed = 4.0f, int max_jumps = 1, int energy = 300) {
	auto avatar = GameNounManager.GetAvatar();

	if (CanDoubleJump(avatar, max_jumps) && avatar->mEnergy >= energy) {
		//--------------------------------------------------------
		
		// remove queued jump burst reset
		if (mJumpReset && !mJumpReset->HasExecuted()) {
			Simulator::RemoveScheduledTask(mJumpReset);
		}

		can_jumpburst = false;
		jumpcount++;

		avatar->mbSupported = true;
		avatar->DoJump(energy);

		//// NEGATE THE DOWNWARDS SPEED!
		// Get the avatar's up and forward vectors
		Vector3 dirUp = avatar->GetPosition().Normalized();
		Vector3 dirFwd = avatar->GetDirection();

		// Get the avatar's current velocity vector
		Vector3 velocity = avatar->GetVelocity();

		// Calculate the speed only in the up direction
		float speedInDirection = avatar->GetVelocity().Dot(dirUp);
		// combine the up and forward components into a final additive velocity to apply
		Vector3 velocityAdditive = (dirUp * -speedInDirection) + (dirFwd * fwd_speed);

		avatar->SetVelocity(velocity + velocityAdditive);

		return true;
		//--------------------------------------------------------
	}
	return false;
}


//-----------------------------------------------------------------------------------------------

bool EP1_CaptainAbilities::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!((IsScenarioMode() && ScenarioMode.GetMode() != App::cScenarioMode::Mode::EditMode) || IsCreatureGame())) { return false; }
	
	// Spacebar
	if (message.Key.vkey == VK_SPACE) {
		// Player has pressed the spacebar
		if (message.IsType(kMsgKeyDown)) {
			return DoubleJump();
		}
		// Player has released the spacebar.
		else if (message.IsType(kMsgKeyUp)) {
			StartBurstResetTimer();
		}
		
	}

	return false;
}

// To fake the cooldown for valid jumps, simply detect when the player has released the SPACE key in the air,
// and start a timer to turn can_jumpburst back on.
void EP1_CaptainAbilities::StartBurstResetTimer() {
	if (!can_jumpburst) {
		mJumpReset = Simulator::ScheduleTask(this, &EP1_CaptainAbilities::ResetAllowJumpBurst, 1.3f);
	}
}

void EP1_CaptainAbilities::ResetAllowJumpBurst() {
	can_jumpburst = true;
}

// when player landed, reset jump count and turn off the jump burst allowance.
void EP1_CaptainAbilities::ResetJumpCount() {
	can_jumpburst = false;
	jumpcount = 0;
}



//-----------------------------------------------------------------------------------------------

// For internal use, do not modify.
int EP1_CaptainAbilities::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EP1_CaptainAbilities::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_CaptainAbilities::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(EP1_CaptainAbilities);
	return nullptr;
}