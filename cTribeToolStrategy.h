#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\cTribeInputStrategy.h>
//#include "TribeToolManager.h"

#define cTribeToolStrategyPtr intrusive_ptr<cTribeToolStrategy>

using namespace UTFWin;
using namespace Simulator;

class cTribeToolStrategy 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IUpdatable
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("cTribeToolStrategy");
	
	cTribeToolStrategy();
	~cTribeToolStrategy();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	virtual void Update();
	//------------------------------------------------------------------

	// These strategies are to be added to a tool strat manager, which can take the tribe tool metadata and assign handle IDs to the tool strategies by key.
	// Basically, if a tool says to use a certain tool strategy key, that strategy will need to have been added to the manager under that name.
	// Then, find that strategy class by its key, and add the handled tool ID from the tool data.
	// Tool metadata is stored in the tool manager class, so it can be accessed globally.

	void AddHandledToolID(int id);
	bool HandlesToolID(int id) const;
	bool HandlesTool(cTribeToolPtr tool) const;

	//------------------------------------------------------------------

	// TODO: add a detour to the tribe creature's DoAction func, to track what creatures are using this tool via their action.
	// it will be complicated, but we must track if a creature gains the action to use this tool, and if so, store them in some way,
	// and reference those when they play the grabbing tool animation (or the chieftain brandish? maybe have an option to disallow chieftain from interact.)
	// in order to either override that animation or play behaviors when they get their item (may need a detour at GetHandheldItem for this).
	// 
	// Also, behaviors should be able to see whether the creature is the first one to use it, or if other creatures are already interacting
	// (meaning queued for grabbing their tool, aka have started playing the grabtool anim but have not yet run the handheld item code.)
	// This will be particularly useful for single-user items, like wishing wells, and for
	// 
	//virtual void
	
	//------------------------------------------------------------------
	// Auto-Called Actions

	/// These return false, unless specifically wanting to handle the mouse unclick input
	virtual bool RightClickedTool(cTribeToolPtr tool);
	virtual bool LeftClickedTool(cTribeToolPtr tool);

	virtual void CreatureHandheldItemChanged(cCreatureCitizenPtr citizen, cTribeToolPtr tool); // Called when the creature's handheld object changes to or from this.
	virtual void CreatureAcquiredTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool); // Called when a creature first acquires this tool.

	//------------------------------------------------------------------
	// User-Called Actions / Getters

	virtual void SetToolState(bool state);
	virtual bool GetToolState() const;

	virtual uint32_t GetInteractAnim(cCreatureCitizenPtr citizen, cTribeToolPtr tool) const;

	//------------------------------------------------------------------
	// Messages

	int GetEventFlags() const override;
	virtual bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	virtual bool HandleMessage(uint32_t messageID, void* msg) override;

public:
	bool mbToolStateValid = true; // Turn off to disallow interaction, and show the invalid cursor when hovered

private:

	// Internal
	bool mbHandleAllTools = false;
	bool mbSingleUserOnly = false; // Only allow one tribe member to interact with this item at a time
	vector<int> mHandledToolIDs = {};

	//cTribeToolManager::ToolMetadata* GetToolMetadata(cTribeToolPtr tool) const;
};
