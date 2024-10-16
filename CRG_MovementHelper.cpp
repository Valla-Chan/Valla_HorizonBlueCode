#include "stdafx.h"
#include "CRG_MovementHelper.h"

CRG_MovementHelper::CRG_MovementHelper()
{
	App::AddUpdateFunction(this);
	WindowManager.GetMainWindow()->AddWinProc(this);
}


CRG_MovementHelper::~CRG_MovementHelper()
{
}

// For internal use, do not modify.
int CRG_MovementHelper::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int CRG_MovementHelper::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* CRG_MovementHelper::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(CRG_MovementHelper);
	return nullptr;
}

void CRG_MovementHelper::Update()
{
	if (CanRun()) {
		TestForStuckMovement();
	}
}

//-----------------------------------------------------------

bool CRG_MovementHelper::CanRun() const {
	return (IsCreatureGame() || (IsScenarioMode() && ScenarioMode.GetMode() == cScenarioMode::Mode::PlayMode));
}

// modified off EP1 Drivemarker
// TODO: probably make this
void CRG_MovementHelper::UpdateMovingFromKey(int vkey, bool keydown) {
	if (!CanRun() || !GameNounManager.GetAvatar()) { mbMovementInputTotal = 0; return; }

	if (keydown) {
		switch (vkey) {
		case 'A':
		case 'D':
		case 'S':
		case 'W':
		case 'Q':
		case 'E': mbMovementInputTotal++; break;
		}
	}
	else {
		switch (vkey) {
		case 'A':
		case 'D':
		case 'S':
		case 'W':
		case 'Q':
		case 'E': mbMovementInputTotal--; break;
		}
	}
	if (mbMovementInputTotal < 0) {
		mbMovementInputTotal = 0;
	}
}


// TODO: make testing much more strict, including a traceline, maybe a normal/gridded height check for the current terrain?
void CRG_MovementHelper::TestForStuckMovement() {
	if (GameTimeManager.IsPaused()) { return; }
	if (mbMovementInputTotal > 0) {
		auto avatar = GameNounManager.GetAvatar();
		if (avatar && avatar->mAge > 0 && !avatar->mbDead && avatar->mbSupported) {

			// Get the avatar's current velocity vector
			Vector3 velocity = avatar->GetVelocity();

			// If avaatar is moving more slowly than desired, they may be climbing a slope!
			if (velocity.Length() < avatar->GetDesiredSpeed() * 0.3) {
				//// NEGATE THE DOWNWARDS SPEED, AND ADD UPWARDS BOOST
				// Get the avatar's up and forward vectors
				Vector3 dirUp = avatar->GetPosition().Normalized();

				

				// Calculate the speed only in the up direction
				float speedInDirection = avatar->GetVelocity().Dot(dirUp);
				// combine the up and forward components into a final additive velocity to apply
				Vector3 velocityAdditive = (dirUp * (-speedInDirection*0.5f + 3.0f)) * (velocity.Length()/ avatar->GetDesiredSpeed());

				avatar->SetVelocity(velocity + velocityAdditive);
			}

		}
	}
}

//-----------------------------------------------------------

int CRG_MovementHelper::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

bool CRG_MovementHelper::HandleUIMessage(IWindow* window, const Message& message)
{
	if (message.IsType(kMsgKeyDown)) {
		UpdateMovingFromKey(message.Key.vkey, true);
	}
	else if (message.IsType(kMsgKeyUp)) {
		UpdateMovingFromKey(message.Key.vkey, false);
	}

	return false;
}
