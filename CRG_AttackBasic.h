#pragma once

#include <Spore\BasicIncludes.h>

#define CRG_AttackBasicPtr intrusive_ptr<CRG_AttackBasic>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;
using namespace Simulator;

class CRG_AttackBasic 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("CRG_AttackBasic");
	
	CRG_AttackBasic();
	~CRG_AttackBasic();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	float GetDistance(Vector3 point1, Vector3 point2) const;

	void AffectClosestInteractable();
	void AffectNearbyCreatures();
	void AffectCreature(cCreatureAnimalPtr creature);


	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
};
