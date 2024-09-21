#include "stdafx.h"
#include "EP1_GameplayObject_DriveMarker.h"
#include "CapabilityChecker.h"
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
		if (mIsDriving && (mCurrentVehicle || mCurrentCreature)) {
			// Exit vehicle if dead
			if (mCurrentVehicle && mCurrentVehicle->mbDead) {
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

// Get the creature mount that is rolled over with the mouse
cCreatureAnimalPtr EP1_GameplayObject_DriveMarker::GetHoveredCreature() {
	auto creatures = Simulator::GetDataByCast<Simulator::cCreatureAnimal>();
	for (auto creature : creatures) {
		if (creature->IsRolledOver()) {
			return creature;
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

cSpatialObjectPtr EP1_GameplayObject_DriveMarker::GetVehicleSpatial() const {
	auto vehicleSpatial = object_cast<cSpatialObject>(mCurrentVehicle);
	if (!vehicleSpatial) {
		vehicleSpatial = object_cast<cSpatialObject>(mCurrentCreature);
	}
	return vehicleSpatial;
}

//------------------------------------------------------------------------------------------------

// TODO: combine these into 1 func with 2 args (1 will be nullptr)
void EP1_GameplayObject_DriveMarker::EnterVehicle(cVehiclePtr vehicle) {
	if (!IsPlayingAdventure() || mDrivingCooldown) { return; }
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		mCurrentVehicle = vehicle;
		mCurrentCreature = nullptr;
		mCurrentVehicle->mStandardSpeed *= 2.0f;
	}
	Enter();
}

// Debug Test
void EP1_GameplayObject_DriveMarker::EnterCreature(cCreatureAnimalPtr creature) {
	if (!IsPlayingAdventure() || mDrivingCooldown) { return; }
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		mCurrentCreature = creature;
		mCurrentVehicle = nullptr;
		mCurrentCreature->mStandardSpeed *= 2.0f;
		
	}
	Enter();
}

void EP1_GameplayObject_DriveMarker::Enter() {
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		mIsDriving = true;
		float scale = avatar->GetScale();
		if (scale != 0.00001f) {
			mPrevAvatarScale = scale;
			mPrevAvatarHealth = avatar->mHealthPoints;
		}
		avatar->mbEnabled = false;
		avatar->mbFixed = true;
		avatar->mbDead = true;
		StartDrivingCooldown();
	}
}

void EP1_GameplayObject_DriveMarker::ExitVehicle() {
	if (mCurrentVehicle || mCurrentCreature) {
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
			
			auto vehicleSpatial = GetVehicleSpatial();

			// Put the avatar to the right of the vehicle.
			auto rightVec = vehicleSpatial->GetPosition().Normalized().Cross(vehicleSpatial->GetDirection());
			auto model = vehicleSpatial->GetModel();
			if (model) {
				auto v_bbox = vehicleSpatial->GetModel()->mDefaultBBox; // vehicle bbox
				auto c_bbox = avatar->GetModel()->mDefaultBBox; // creature bbox
				auto newPos = vehicleSpatial->GetPosition() + (rightVec * (1.3f + abs(v_bbox.lower.x + c_bbox.lower.x)));

				avatar->Teleport(newPos, vehicleSpatial->GetOrientation());
			}
		}
		
		if (mCurrentVehicle) {
			mCurrentVehicle->mTurnRate = Math::PI;
			mCurrentVehicle->mStandardSpeed *= 0.5f;
		}
		else {
			mCurrentCreature->mStandardSpeed *= 0.5f;
		}
		
	}

	mCurrentVehicle = nullptr;
	mCurrentCreature = nullptr;
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
			avatar->mbDead = false;
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
	if ((mCurrentVehicle || mCurrentCreature) && mIsMouseHeld) {
		mDestinationPos = GameViewManager.GetWorldMousePosition();
	}
	else {
		mDestinationPos = Vector3(0, 0, 0);
	}
}

// Use mVecInput to determine where the vehicle should drive to
void EP1_GameplayObject_DriveMarker::SetDestinationFromInput() {
	if ((mCurrentVehicle || mCurrentCreature) && mVecInput != Vector2(0, 0)) {
		auto inputDir = mVecInput.Normalized();

		auto pViewer = CameraManager.GetViewer();

		Vector3 camPos = Vector3(0,0,0);
		Vector3 camDir = Vector3(0,0,0);

		// Get vector to the center of the screen.
		auto windowArea = WindowManager.GetMainWindow()->GetArea();
		pViewer->GetCameraToPoint(windowArea.right/2.0f, windowArea.bottom/2.0f, camPos, camDir);

		Vector3 currentVehiclePos;
		Vector3 currentVehicleDir;
		Vector3 currentVehicleVelocity;
		if (mCurrentVehicle) {
			currentVehiclePos = mCurrentVehicle->GetPosition();
			currentVehicleDir = mCurrentVehicle->GetDirection();
			currentVehicleVelocity = mCurrentVehicle->GetVelocity();
		}
		else {
			currentVehiclePos = mCurrentCreature->GetPosition();
			currentVehicleDir = mCurrentCreature->GetDirection();
			currentVehicleVelocity = mCurrentCreature->GetVelocity();
		}
		
		

		// create a temp pos from the camera orientation, then get the vehicle direction to that pos.
		Vector3 tempPos = currentVehiclePos + (camDir * 15.0f);
		Vector3 dir_to_temp_pos = (tempPos - currentVehiclePos).Normalized();

		// find the side dir vector
		auto rightVec = currentVehiclePos.Normalized().Cross(currentVehicleDir);

		// create destination from input directions
		mDestinationPos = currentVehiclePos;
		float sign = 1;
		if (mVecInput.y < 0) {
			sign = -1;
		}
		mDestinationPos += (dir_to_temp_pos * 50.0f * (mVecInput.y)) + (rightVec * 30.0f * -mVecInput.x * sign); //.y + (abs(mVecInput.x*0.2f)
		
		// Modulate turn rate based on input
		auto turnRate = min(Math::PI * (1.1f + float(mVecInput.y < 1) + 1 - abs(mVecInput.x)), Math::PI * 2);
		// if there is only side inputs, and the car has significant velocity, drop the turn rate.
		if (mVecInput.x != 0 && mVecInput.y == 0 && currentVehicleVelocity.Length() > 30.0f) {
			turnRate *= 0.95f;
		}

		if (mCurrentVehicle) {
			mCurrentVehicle->mTurnRate = turnRate;
		}
		
	}
}

// Drive to mDestinationPos.
void EP1_GameplayObject_DriveMarker::DriveToDestination() {
	if ((mCurrentVehicle || mCurrentCreature) && mDestinationPos != Vector3(0, 0, 0)) {
		if (mCurrentVehicle) {
			mCurrentVehicle->MoveTo(mDestinationPos);
		} else {
			mCurrentCreature->WalkTo(1, mDestinationPos, mDestinationPos, 1.0f, 1.0f);
			mCurrentCreature->MoveTo(mDestinationPos);
		}
		

		if (mVecInput.y != 1.0f) {
			Vector3 dir;
			Vector3 velocity;
			if (mCurrentVehicle) {
				dir = mDestinationPos - mCurrentVehicle->GetPosition();
				velocity = mCurrentVehicle->GetVelocity();
			}
			else {
				dir = mDestinationPos - mCurrentCreature->GetPosition();
				velocity = mCurrentCreature->GetVelocity();
			}

			Vector3 newVelocity = velocity;
			if (velocity.Length() > 10.0f) {
				newVelocity = velocity + (dir * 0.01f);
			}
			else {
				newVelocity = velocity + (dir * 0.05f);
			}
			// Apply
			if (mCurrentVehicle) {
				mCurrentVehicle->SetVelocity(newVelocity);
			}
			else {
				mCurrentCreature->SetVelocity(velocity + (dir * 0.01f));
			}
			
		}
		
	}
}


void EP1_GameplayObject_DriveMarker::TeleportAvatarToVehicle() {
	auto avatar = GameNounManager.GetAvatar();
	if (avatar) {
		avatar->mbIsGhost = true;
		if (mPrevAvatarHealth > 0.0f) {
			avatar->mHealthPoints = mPrevAvatarHealth;
		}

		auto vehicleSpatial = GetVehicleSpatial();

		// Get the vehicle's up vector and bbox
		Vector3 dirUp = vehicleSpatial->GetPosition().Normalized();
		auto bbox = vehicleSpatial->GetModel()->mDefaultBBox;

		Vector3 newPos = vehicleSpatial->GetPosition() + (dirUp * bbox.upper.z * vehicleSpatial->GetScale());


		if (mCurrentCreature) {
			// attach to creature's back
			auto partoffset = CapabilityChecker.GetPosFromPartCapability(mCurrentCreature, 0xB00F0FEC); // ModelCapabilityMouth
			newPos = vehicleSpatial->GetPosition() + partoffset;
		}
		else {
			// Hide avatar inside the vehicle so the camera still follows it
			avatar->SetScale(0.00001f);
		}
		avatar->Teleport(newPos, vehicleSpatial->GetOrientation());


	}
}

// TODO: only run this when not holding down the right/middle mouse!
// TODO: this is not working :(
void EP1_GameplayObject_DriveMarker::OrientVehicleCamera() {
	if ((mCurrentVehicle || mCurrentCreature) && mDestinationPos != Vector3(0, 0, 0)) {
		// save original transform
		auto pViewer = CameraManager.GetViewer();
		auto cameraOldTransform = pViewer->GetViewTransform();
		// Get the vehicle's up vector
		Vector3 dirUp = GetVehicleSpatial()->GetPosition().Normalized();
		// rotate the camera to face the destination
		cameraOldTransform.SetRotation(Matrix3::LookAt(cameraOldTransform.GetOffset(), mDestinationPos, dirUp));
		pViewer->SetCameraTransform(cameraOldTransform);

	}
}


// If the vehicle should stop, stop it.
void EP1_GameplayObject_DriveMarker::CheckAndStop() {
	if (mDestinationPos == Vector3(0, 0, 0) && mVecInput == Vector2(0,0)) {
		if (mCurrentVehicle) {
			mCurrentVehicle->StopMovement();
		}
		else if (mCurrentCreature) {
			mCurrentCreature->StopMovement();
		}
		
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
				else if (mCurrentCreature && GetHoveredCreature() == mCurrentCreature) {
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
			auto creature = GetHoveredCreature();
			if (vehicle && IsVehicleDrivable(vehicle)) {
				EnterVehicle(vehicle);
				return true;
			}
			else if (creature) {
				EnterCreature(creature);
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