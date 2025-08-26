#include "stdafx.h"
#include "ArenaMode.h"

ArenaMode::ArenaMode()
{
	Initialize(nullptr);
}

ArenaMode::~ArenaMode()
{

}

// For internal use, do not touch these.
int ArenaMode::AddRef() { return DefaultRefCounted::AddRef(); }
int ArenaMode::Release() { return DefaultRefCounted::Release(); }

// The use of these functions are unknown.
bool ArenaMode::func0Ch() { return false; }
void* ArenaMode::func20h(int) { return nullptr; }


bool ArenaMode::Initialize(App::IGameModeManager* pManager)
{
	CameraManager.SetActiveCameraByID(id("EffectEditorCamera"));

	mpModelWorld = ModelManager.CreateWorld(MODE_ID);
	mpLightingWorld = LightingManager.CreateLightingWorld(MODE_ID);
	mpEffectWorld = EffectsManager.CreateWorld(MODE_ID);
	mpAnimWorld = AnimManager.CreateWorld(u"ArenaMode");
	EffectsManager.SetDefaultWorld(mpEffectWorld.get()); // prev swarm SetActiveWorld

	mpAnimWorld->SetEffectWorld(mpEffectWorld.get());
	mpAnimWorld->SetModelWorld(mpModelWorld.get());

	//TODO sub_7826A0 related with shadows? check end of Editor_Update
	//mpLightingWorld->SetConfiguration(id("SandboxMode"));
	//mpModelWorld->AddLightingWorld(mpLightingWorld.get(), 0, false);
	//mpModelWorld->SetVisible(true);

	//RenderManager.AddRenderable(mpModelWorld->ToRenderable(), 8);

	//CameraManager.SetActiveCameraByID(id("EffectEditorCamera"));
	//CameraManager.SetActiveCameraByID(SphereCamera::ID);
	//mpCamera = (SphereCamera*)CameraManager.GetActiveCamera();

	CreateCheat();

	return true;
}

void ArenaMode::CreateCheat() {
	class Cheat_Enter : public ArgScript::ICommand
	{
	public:
		void ParseLine(const ArgScript::Line& line) override {
			GameModeManager.SetActiveMode(MODE_ID);
		}
	};

	// Add Cheats
	CheatManager.AddCheat("enter_arena", new Cheat_Enter());
}

bool ArenaMode::Dispose()
{
	// Called when the game exits. Here you should dispose all your model/effect worlds,
	// ensure all objects are released, etc
	
	return true;
}

bool ArenaMode::OnEnter()
{
	
	return true;
}

void ArenaMode::OnExit()
{
	
}

//------------------------------------------------------------------------
//// SPOREPEDIA LISTENERS ////

void ArenaMode::OnShopperAccept(const ResourceKey& selection)
{
	//AddCreature(selection);
	//PlayAnimation(ANIM_ID, 0);
}


//------------------------------------------------------------------------
//// INPUT LISTENERS ////

// Called when a keyboard key button is pressed.
bool ArenaMode::OnKeyDown(int virtualKey, KeyModifiers modifiers)
{
	mInput.OnKeyDown(virtualKey, modifiers);
	
	return false;
}

// Called when a keyboard key button is released.
bool ArenaMode::OnKeyUp(int virtualKey, KeyModifiers modifiers)
{
	mInput.OnKeyUp(virtualKey, modifiers);
	
	return false;
}

// Called when a mouse button is pressed (this includes the mouse wheel button).
bool ArenaMode::OnMouseDown(MouseButton mouseButton, float mouseX, float mouseY, MouseState mouseState)
{
	mInput.OnMouseDown(mouseButton, mouseX, mouseY, mouseState);
	
	return false;
}

// Called when a mouse button is released (this includes the mouse wheel button).
bool ArenaMode::OnMouseUp(MouseButton mouseButton, float mouseX, float mouseY, MouseState mouseState)
{
	mInput.OnMouseUp(mouseButton, mouseX, mouseY, mouseState);
	
	return false;
}

// Called when the mouse is moved.
bool ArenaMode::OnMouseMove(float mouseX, float mouseY, MouseState mouseState)
{
	mInput.OnMouseMove(mouseX, mouseY, mouseState);
	
	return false;
}

// Called when the mouse wheel is scrolled. 
// This method is not called when the mouse wheel is pressed.
bool ArenaMode::OnMouseWheel(int wheelDelta, float mouseX, float mouseY, MouseState mouseState)
{
	mInput.OnMouseWheel(wheelDelta, mouseX, mouseY, mouseState);
	
	return false;
}


//// UPDATE FUNCTION ////

void ArenaMode::Update(float delta1, float delta2)
{
}
