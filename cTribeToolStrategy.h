#pragma once

#include <Spore\BasicIncludes.h>

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
	//-----------------------------------------

	// These strategies are to be added to a tool strat manager, which can take the tribe tool metadata and assign handle IDs to the tool strategies by key.
	// Basically, if a tool says to use a certain tool strategy key, that strategy will need to have been added to the manager under that name.
	// Then, find that strategy class by its key, and add the handled tool ID from the tool data.
	// Tool metadata is stored in the tool manager class, so it can be accessed globally.

	void AddHandledToolID(int id);
	bool HandlesToolID(int id) const;
	bool HandlesTool(cTribeToolPtr tool) const;

	//----------------------------------------------------------------

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

	/// These return false, unless specifically wanting to handle the mouse unclick input
	virtual bool RightClickedTool(cTribeToolPtr tool);
	virtual bool LeftClickedTool(cTribeToolPtr tool);

	void CreatureHandheldItemChanged(cCreatureCitizenPtr citizen); // TODO: Called when the creature's handheld object changes. calls CreatureAcquiredTool
	void CreatureAcquiredTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool);

	int GetEventFlags() const override;
	virtual bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	virtual bool HandleMessage(uint32_t messageID, void* msg) override;

private:

	struct MouseButtonsState {
		bool l = false;
		bool r = false;
		bool interrupted = false;

		MouseButtonsState(bool lstate, bool rstate) {
			l = lstate;
			r = rstate;
		}
		void SetState(MouseButton button, bool state) {
			if (button == MouseButton::kMouseButtonLeft) {
				l = state;
			}
			else if (button == MouseButton::kMouseButtonRight) {
				r = state;
			}
			// if both buttons have become active, set the input as interrupted.
			if (l && r) {
				interrupted = true;
			}
		}
		void Interrupt() {
			interrupted = true;
		}
		bool AreAnyPressed() {
			return (l || r);
		}
		bool IsRightMousePressed() {
			return (r && !l);
		}
		bool IsLeftMousePressed() {
			return (r && !l);
		}
		void Clear() {
			l = false;
			r = false;
		}
		void Reset() {
			Clear();
			interrupted = false;
		}
	};

	bool mbHandleAllTools = false;
	bool mbSingleUserOnly = false; // Only allow one tribe member to interact with this item at a time
	vector<int> mHandledToolIDs = {};
	MouseButtonsState mMouseButtonsPressed = { false, false };
	cTribeToolPtr mpStoredObject = nullptr; // tool object to check if still hovering
};
