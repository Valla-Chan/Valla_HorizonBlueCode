#pragma once

#include <Spore\BasicIncludes.h>

#define CRG_NestManagerPtr intrusive_ptr<CRG_NestManager>

using namespace Simulator;

class CRG_NestManager 
	: public App::IMessageListener
	, public DefaultRefCounted
	, public App::IUpdatable
{
public:
	static const uint32_t TYPE = id("CRG_NestManager");

	CRG_NestManager();
	~CRG_NestManager();

	int AddRef() override;
	int Release() override;
	void Update() override;
	
	void CheckNestForExtinction();

	// This is the function you have to implement, called when a message you registered to is sent.
	bool HandleMessage(uint32_t messageID, void* message) override;
private:
	cNestPtr lastNest;
};
