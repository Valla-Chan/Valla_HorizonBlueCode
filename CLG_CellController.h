#pragma once

#include <Spore\BasicIncludes.h>

#define CLG_CellControllerPtr intrusive_ptr<CLG_CellController>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;
using namespace Simulator::Cell;

class CLG_CellController 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("CLG_CellController");
	
	CLG_CellController();
	~CLG_CellController();

	bool mbStateStealthed = false;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	void Update() override;

	void UpdatePlayerHealth();
	
	int GetJetLvl() const;
	void JetBurst();
	void ToggleStealth(bool active);
	void Pheromone(bool attract, float radius = 4.0f, float time = 6.0f);

	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

private:
	int mCurrentPlayerHealth = 6;
	int mPrevPlayerHealth = 6;
};
