#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\cInteractiveOrnament.h>
#include <Spore\App\IMessageListener.h>

#define ObjectManager  (*cObjectManager::Get())
#define cObjectManagerPtr intrusive_ptr<cObjectManager>

using namespace Simulator;

class cObjectManager
	: public Object
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("cObjectManager");
	cObjectManager();
	~cObjectManager();
	

	//--------------------
	// object models
	const uint32_t mdl_carcass01 = id("CRG_Food_Carcass_01");
	// animations
	const uint32_t anim_dig = 0x03DF6DFF;
	const uint32_t anim_eat_meat_mouth = 0x04F65995;
	//--------------------

	bool waiting_for_noun = false;

	cInteractiveOrnament* GetSelectedObject() const;
	bool AvatarIsCarnivore() const;
	void StartWaitingForNoun();


	bool HandleMessage(uint32_t messageID, void* msg) override;

	static cObjectManager* Get();
	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

protected:
	static cObjectManager* sInstance;

private:
	intrusive_ptr<App::ScheduledTaskListener> mWaitTask;
};
