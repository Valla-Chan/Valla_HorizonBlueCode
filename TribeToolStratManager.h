#pragma once

#include <Spore\BasicIncludes.h>
#include "cTribeToolStrategy.h"
#include "TribeToolDataKeys.h"

#define cTribeToolStratManagerPtr intrusive_ptr<cTribeToolStratManager>
#define TribeToolStratManager (*cTribeToolStratManager::Get())
#define ToolMetadataPtr (intrusive_ptr<ToolMetadata>)

using namespace UTFWin;
using namespace Simulator;

class cTribeToolStratManager 
	: public IWinProc
	, public DefaultRefCounted
	, public App::IMessageListener
{
public:
	static const uint32_t TYPE = id("TribeToolStratManager");
	
	cTribeToolStratManager();
	~cTribeToolStratManager();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	static cTribeToolStratManager* Get();

	//------------------------------------------------------------
	// Strategies
	//------------------------------------------------------------
	hash_map<int, uint32_t> mTribeToolStrategies; // map toolType ID to strategy ID
	hash_map<uint32_t, cTribeToolStrategyPtr> mStrategies;

	void AddStrategy(cTribeToolStrategy* pStrategy, uint32_t strategyID);
	cTribeToolStrategyPtr GetStrategy(uint32_t strategyID);

	cTribeToolStrategyPtr GetStrategyForToolType(int toolID);
	// Associate this tool ID with this strategy ID.
	void SetToolIDStrategy(int toolID, uint32_t strategyID);


protected:
	static cTribeToolStratManager* sInstance;

	//------------------------------------------------------------
	// Mouse Inputs

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
	cTribeToolPtr mpStoredTool = nullptr; // tool object to check if still hovered

	virtual bool HandleToolMouseMessage(IWindow* pWindow, const Message& message);

	int GetEventFlags() const override;
	virtual bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	virtual bool HandleMessage(uint32_t messageID, void* msg) override;

private:
	// Called automatically. Adds all the default tool strategies.
	void AddBaseStrategies() {
		TribeToolStratManager.AddStrategy(new cTribeToolStrategy(), id("TribeToolStrategy"));
	}
};
