#pragma once

#include <Spore\BasicIncludes.h>
#include "ArenaCreature.h"

#define ArenaModePtr intrusive_ptr<ArenaMode>

class ArenaMode 
	: public App::IGameMode
	, public Sporepedia::IShopperListener
	//, public Graphics::ILayer
	, public DefaultRefCounted
{
public:
	static const uint32_t MODE_ID = id("ArenaMode");

	ArenaMode();

	int AddRef() override;
	int Release() override;
	~ArenaMode();

	bool func0Ch() override;
	void* func20h(int) override;
	bool Initialize(App::IGameModeManager* pManager) override;
	void CreateCheat();
	bool Dispose() override;
	bool OnEnter() override;
	void OnExit() override;
	
	void OnShopperAccept(const ResourceKey& selection) override;

	bool OnKeyDown(int virtualKey, KeyModifiers modifiers) override;
	bool OnKeyUp(int virtualKey, KeyModifiers modifiers) override;
	bool OnMouseDown(MouseButton mouseButton, float mouseX, float mouseY, MouseState mouseState) override;
	bool OnMouseUp(MouseButton mouseButton, float mouseX, float mouseY, MouseState mouseState) override;
	bool OnMouseMove(float mouseX, float mouseY, MouseState mouseState) override;
	bool OnMouseWheel(int wheelDelta, float mouseX, float mouseY, MouseState mouseState) override;
	void Update(float delta1, float delta2) override;
	
protected:

	GameInput mInput;

	IModelWorldPtr mpModelWorld;
	ILightingWorldPtr mpLightingWorld;
	IEffectsWorldPtr mpEffectWorld;
	IAnimWorldPtr mpAnimWorld;

	struct Arena {
		ResourceKey mArenaKey; // Arena resource definition

		float mBoundsRadius = 5.0f; // placeholder
		ResourceKey mArenaEffect;
		ResourceKey mArenaThumb;
		
	};

	Arena mArena;

	// TODO: eventually replace these with a ptr to a custom class that holds an AnimatedCreaturePtr AND info about health, current action, NPC vs player designation, etc
	ArenaCreaturePtr mpCrtPlayer;
	ArenaCreaturePtr mpCrtEnemy;
};
