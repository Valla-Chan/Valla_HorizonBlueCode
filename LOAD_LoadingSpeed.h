#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\App\IMessageListener.h>

#define LOAD_LoadingSpeedPtr intrusive_ptr<LOAD_LoadingSpeed>

class LOAD_LoadingSpeed 
	: public Object
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("LOAD_LoadingSpeed");
	
	LOAD_LoadingSpeed();
	~LOAD_LoadingSpeed();

	bool HandleMessage(uint32_t messageID, void* msg) override;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
