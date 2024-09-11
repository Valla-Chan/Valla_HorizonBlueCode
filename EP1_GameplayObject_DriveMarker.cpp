#include "stdafx.h"
#include "EP1_GameplayObject_DriveMarker.h"
#include <Spore\Input.h>

EP1_GameplayObject_DriveMarker::EP1_GameplayObject_DriveMarker()
{
	App::AddUpdateFunction(this);
}


EP1_GameplayObject_DriveMarker::~EP1_GameplayObject_DriveMarker()
{
}

void EP1_GameplayObject_DriveMarker::Update()
{
	if (IsPlayingAdventure()) {
		if (mIsDriving && mCurrentVehicle) {
			// Exit vehicle if dead
			if (mCurrentVehicle->mbDead) {
				ExitVehicle();
			}

			// Drive to mouse pos if mouse is held
			SetDestinationFromCursor();
			// Calculate new pos from key inputs
			SetDestinationFromInput();

			// Drive to pos
			DriveToDestination();

			TeleportAvatarToVehicle();
			OrientVehicleCamera();

			CheckAndStop();
		}
	}
	else if (mIsDriving) {
		ExitVehicle();
	}
}

//------------------------------------------------------------------------------------------------

bool EP1_GameplayObject_DriveMarker::IsPlayingAdventure() {
	return (IsScenarioMode() && ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode);
}

// Get the vehicle that is rolled over with the mouse
cVehiclePtr EP1_GameplayObject_DriveMarker::GetHoveredVehicle() {
	auto vehicles = Simulator::GetDataByCast<Simulator::cVehicle>();
	for (auto vehicle : vehicles) {
		if (vehicle->IsRolledOver()) {
			return vehicle;
		}
	}
	return nullptr;
}

// If a vehicle is in the drivable list
bool EP1_GameplayObject_DriveMarker::IsVehicleDrivable(cVehiclePtr vehicle) const {
	// DEBUG
	return true;
	
	for (auto item : mDrivableVehicles) {
		return vehicle == item;
	}
}

// Get how fast the current vehicle should move
// TODO: make this work well.
float EP1_GameplayObject_DriveMarker::GetCurrentVehicleSpeed() const {
	return 5.0f;

	if (mCurrentVehicle) {
		return mCurrentVehicle->GetStandardSpeed() * mCurrentVehicle->mSpeedStat;
	}
	return 1.0;
}

//------------------------------------------------------------------------------------------------

void EP1_GameplayObject_DriveMarker::EnterVehicle(cVehiclePtr vehicle) {
	if (!IsPlayingAdventure() || mDrivingCooldown) { return; }
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		mCurrentVehicle = vehicle;
		mIsDriving = true;
		float scale = avatar->GetScale();
		if (scale != 0.00001f) {
			mPrevAvatarScale = scale;
			mPrevAvatarHealth = avatar->mHealthPoints;
		}
		avatar->mbEnabled = false;
		avatar->mbFixed = true;
		mCurrentVehicle->mStandardSpeed *= 2.0f;

		StartDrivingCooldown();
	}
}

void EP1_GameplayObject_DriveMarker::ExitVehicle() {
	if (mCurrentVehicle) {
		auto avatar = GameNounManager.GetAvatar();
		if (avatar && IsPlayingAdventure()) {
			StartDrivingCooldown();

			if (mPrevAvatarScale != 0.00001f) {
				avatar->SetScale(mPrevAvatarScale);
			}
			// failsafe for if the scale has somehow gotten stuck on the low number
			else {
				avatar->SetScale(1.0f);
				mPrevAvatarScale = 1.0f;
			}
			avatar->mbSupported = false;
			

			// Put the avatar to the right of the vehicle.
			auto rightVec = mCurrentVehicle->GetPosition().Normalized().Cross(mCurrentVehicle->GetDirection());
			auto model = mCurrentVehicle->GetModel();
			if (model) {
				auto v_bbox = mCurrentVehicle->GetModel()->mDefaultBBox; // vehicle bbox
				auto c_bbox = avatar->GetModel()->mDefaultBBox; // creature bbox
				auto newPos = mCurrentVehicle->GetPosition() + (rightVec * (1.3f + abs(v_bbox.lower.x + c_bbox.lower.x)));

				avatar->Teleport(newPos, mCurrentVehicle->GetOrientation());
			}
		}
		mCurrentVehicle->mStandardSpeed *= 0.5f;
		mCurrentVehicle->mTurnRate = Math::PI;
	}

	mCurrentVehicle = nullptr;
	mIsMouseHeld = false;
	mIsDriving = false;
	auto task = Simulator::ScheduleTask(this, &EP1_GameplayObject_DriveMarker::ExitVehicle_Delayed, 0.2f);
}


void EP1_GameplayObject_DriveMarker::ExitVehicle_Delayed() {
	if (IsScenarioMode()) {
		auto avatar = GameNounManager.GetAvatar();
		if (avatar) {
			avatar->mbIsGhost = false;
			avatar->mbEnabled = true;
			avatar->mbFixed = false;
		}
	}
}

void EP1_GameplayObject_DriveMarker::StartDrivingCooldown() {
	if (IsPlayingAdventure()) {
		mDrivingCooldown = true;
		if (mTask && !mTask->HasExecuted()) {
			Simulator::RemoveScheduledTask(mTask);
		}
		mTask = Simulator::ScheduleTask(this, &EP1_GameplayObject_DriveMarker::EndDrivingCooldown, 0.5f);
	}
}

void EP1_GameplayObject_DriveMarker::EndDrivingCooldown() {
	mDrivingCooldown = false;
}

// Map keys to mVecInputRaw values, and update mVecInput
bool EP1_GameplayObject_DriveMarker::UpdateVecFromKey(int vkey, bool keydown) {
	bool validkey = false;
	if (keydown) {
		switch (vkey) {
			case 'A': mVecInputRaw.x = 1; validkey = true; break;
			case 'D': mVecInputRaw.y = 1; validkey = true; break;
			case 'S': mVecInputRaw.z = 1; validkey = true; break;
			case 'W': mVecInputRaw.w = 1; validkey = true; break;
		}
	}
	else {
		switch (vkey) {
			case 'A': mVecInputRaw.x = 0; validkey = true; break;
			case 'D': mVecInputRaw.y = 0; validkey = true; break;
			case 'S': mVecInputRaw.z = 0; validkey = true; break;
			case 'W': mVecInputRaw.w = 0; validkey = true; break;
		}
	}
	mVecInput.x = mVecInputRaw.y - mVecInputRaw.x;
	mVecInput.y = mVecInputRaw.w - mVecInputRaw.z;

	// if there is a direction, clear the previous destination.
	if (mVecInput.x != 0 || mVecInput.y != 0) {
		mDestinationPos = Vector3(0, 0, 0);
	}
	return validkey;
}

// Set the mDestinationPos from the cursor world pos, if the mouse button is held down.
void EP1_GameplayObject_DriveMarker::SetDestinationFromCursor() {
	if (mCurrentVehicle && mIsMouseHeld) {
		mDestinationPos = GameViewManager.GetWorldMousePosition();
	}
	else {
		mDestinationPos = Vector3(0, 0, 0);
	}
}

// Use mVecInput to determine where the vehicle should drive to
void EP1_GameplayObject_DriveMarker::SetDestinationFromInput() {
	if (mCurrentVehicle && mVecInput != Vector2(0, 0)) {
		auto inputDir = mVecInput.Normalized();

		auto pViewer = CameraManager.GetViewer();

		Vector3 camPos = Vector3(0,0,0);
		Vector3 camDir = Vector3(0,0,0);

		// Get vector to the center of the screen.
		auto windowArea = WindowManager.GetMainWindow()->GetArea();
		pViewer->GetCameraToPoint(windowArea.right/2.0f, windowArea.bottom/2.0f, camPos, camDir);

		// create a temp pos from the camera orientation, then get the vehicle direction to that pos.
		Vector3 tempPos = mCurrentVehicle->mPosition + (camDir * 15.0f);
		Vector3 dir_to_temp_pos = (tempPos - mCurrentVehicle->GetPosition()).Normalized();

		// find the side dir vector
		auto rightVec = mCurrentVehicle->GetPosition().Normalized().Cross(mCurrentVehicle->GetDirection());

		// create destination from input directions
		mDestinationPos = mCurrentVehicle->mPosition;
		float sign = 1;
		if (mVecInput.y < 0) {
			sign = -1;
		}
		mDestinationPos += (dir_to_temp_pos * 50.0f * (mVecInput.y)) + (rightVec * 30.0f * -mVecInput.x * sign); //.y + (abs(mVecInput.x*0.2f)
		
		// Modulate turn rate based on input
		auto turnRate = min(Math::PI * (1.1f + float(mVecInput.y < 1) + 1 - abs(mVecInput.x)), Math::PI * 2);
		// if there is only side inputs, and the car has significant velocity, drop the turn rate.
		if (mVecInput.x != 0 && mVecInput.y == 0 && mCurrentVehicle->GetVelocity().Length() > 30.0f) { 
			turnRate *= 0.95f;
		}
		mCurrentVehicle->mTurnRate = turnRate;
	}
}

// Drive to mDestinationPos.
void EP1_GameplayObject_DriveMarker::DriveToDestination() {
	if (mCurrentVehicle && mDestinationPos != Vector3(0, 0, 0)) {
		mCurrentVehicle->MoveTo(mDestinationPos);

		if (mVecInput.y != 1.0f) {
			auto dir = mDestinationPos - mCurrentVehicle->GetPosition();

			auto velocity = mCurrentVehicle->GetVelocity();
			if (velocity.Length() > 10.0f) {
				mCurrentVehicle->SetVelocity(velocity + (dir * 0.01f));
			}
			else {
				mCurrentVehicle->SetVelocity(velocity + (dir * 0.05f));
			}
			
		}
		
	}
}


void EP1_GameplayObject_DriveMarker::TeleportAvatarToVehicle() {
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->SetScale(0.00001f);
		avatar->mbIsGhost = true;
		if (mPrevAvatarHealth > 0.0f) {
			avatar->mHealthPoints = mPrevAvatarHealth;
		}

		// Get the vehicle's up vector and bbox
		Vector3 dirUp = mCurrentVehicle->GetPosition().Normalized();
		auto bbox = mCurrentVehicle->GetModel()->mDefaultBBox;

		Vector3 newPos = mCurrentVehicle->GetPosition() + (dirUp * bbox.upper.z * mCurrentVehicle->GetScale());

		// Hide avatar inside the vehicle so the camera still follows it
		avatar->Teleport(newPos, mCurrentVehicle->GetOrientation());

	}
}

// TODO: only run this when not holding down the right/middle mouse!
// TODO: this is not working :(
void EP1_GameplayObject_DriveMarker::OrientVehicleCamera() {
	if (mCurrentVehicle && mDestinationPos != Vector3(0, 0, 0)) {
		// save original transform
		auto pViewer = CameraManager.GetViewer();
		auto cameraOldTransform = pViewer->GetViewTransform();
		// Get the vehicle's up vector
		Vector3 dirUp = mCurrentVehicle->GetPosition().Normalized();
		// rotate the camera to face the destination
		cameraOldTransform.SetRotation(Matrix3::LookAt(cameraOldTransform.GetOffset(), mDestinationPos, dirUp));
		pViewer->SetCameraTransform(cameraOldTransform);

	}
}


// If the vehicle should stop, stop it.
void EP1_GameplayObject_DriveMarker::CheckAndStop() {
	if (mCurrentVehicle && mDestinationPos == Vector3(0, 0, 0) && mVecInput == Vector2(0,0)) {
		mCurrentVehicle->StopMovement();
	}
}

//------------------------------------------------------------------------------------------------


int EP1_GameplayObject_DriveMarker::GetEventFlags() const
{
	return kEventFlagBasicInput;
}


bool EP1_GameplayObject_DriveMarker::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsPlayingAdventure()) { return false; }

	// Driving
	if (mIsDriving) {
		// Keypress
		if (message.IsType(kMsgKeyDown)) {
			return UpdateVecFromKey(message.Key.vkey, true);
		}
		else if (message.IsType(kMsgKeyUp)) {
			return UpdateVecFromKey(message.Key.vkey, false);
		}
		// Click
		else if (message.Mouse.IsLeftButton()) {
			if (message.IsType(kMsgMouseDown)) {
				// Clicked on own vehicle, therefore exit it.
				if (mCurrentVehicle && GetHoveredVehicle() == mCurrentVehicle) {
					ExitVehicle();
				}
				else {
					mIsMouseHeld = true;
				}
				return true;

			}
		}
		if (message.IsType(kMsgMouseUp) && mIsMouseHeld) {
			mIsMouseHeld = false;
			return true;
		}
	}
	// Not Driving
	else {
		if (message.Mouse.IsLeftButton()) {
			// Clicked on a drivable vehicle, therefore enter it.
			auto vehicle = GetHoveredVehicle();
			if (vehicle && IsVehicleDrivable(vehicle)) {
				EnterVehicle(vehicle);
				return true;
			}
		}
	}
	
	return false;
}


//------------------------------------------------------------------------------------------------

// For internal use, do not modify.
int EP1_GameplayObject_DriveMarker::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EP1_GameplayObject_DriveMarker::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EP1_GameplayObject_DriveMarker::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(EP1_GameplayObject_DriveMarker);
	return nullptr;
}