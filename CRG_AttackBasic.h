#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\App\IMessageListener.h>

#define CRG_AttackBasicPtr intrusive_ptr<CRG_AttackBasic>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;

class CRG_AttackBasic 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("CRG_AttackBasic");
	
	CRG_AttackBasic();
	~CRG_AttackBasic();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	const float attackrange = 5.0f;
	bool attacking = false; // TODO: set this back to false on a timer as well, so if the animation does not complete then it can still be used again

	void AffectClosestInteractable();
	void AffectNearbyCreatures();
	void AffectCreature(cCreatureAnimalPtr creature);


	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	bool HandleMessage(uint32_t messageID, void* msg) override;
	
};
