#include "stdafx.h"
#include "TribeToolStratManager.h"

cTribeToolStratManager::cTribeToolStratManager()
{
	sInstance = this;
}


cTribeToolStratManager::~cTribeToolStratManager()
{
	sInstance = nullptr;
}

cTribeToolStratManager* cTribeToolStratManager::sInstance;

cTribeToolStratManager* cTribeToolStratManager::Get()
{
	return sInstance;
}

// For internal use, do not modify.
int cTribeToolStratManager::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cTribeToolStratManager::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cTribeToolStratManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(cTribeToolStratManager);
	return nullptr;
}

//-------------------------------------------------------------------------------

void cTribeToolStratManager::AddStrategy(cTribeToolStrategy* pStrategy, uint32_t id)
{
	mStrategies[id] = pStrategy;
}

cTribeToolStrategyPtr cTribeToolStratManager::GetStrategy(uint32_t id)
{
	auto E = mStrategies.find(id);
	if (E != mStrategies.end()) {
		return E.get_node()->mValue.second;
	}
}

//-------------------------------------------------------------------------------

cTribeToolPtr cTribeToolStratManager::GetHoveredTool(bool playerOwned) {
	auto hoveredTool = object_cast<Simulator::cTribeTool>(GameViewManager.GetHoveredObject());
	if (playerOwned) {
		if (hoveredTool && hoveredTool->mTribe == GameNounManager.GetPlayerTribe()) {
			return hoveredTool;
		}
		else {
			return nullptr;
		}
	}
	return hoveredTool;
}

// TODO: may need to access tribe tool metadata.
bool cTribeToolStratManager::CanToolBeUsed(cTribeToolPtr tool) {
	return true;
	//tool->data
}


vector<cCreatureCitizenPtr> cTribeToolStrategy::GetSelectedMembers() {
	vector<cCreatureCitizenPtr> selected;
	auto tribe = GameNounManager.GetPlayerTribe();
	eastl::vector<cSpatialObjectPtr>& members = tribe->GetSelectableMembers();
	for (auto member : members) {
		auto citizen = object_cast<Simulator::cCreatureCitizen>(member);
		if (member->IsSelected() && citizen) {
			selected.push_back(citizen);
		}
	}
	return selected;
}

void cTribeToolStrategy::UseTool(vector<cCreatureCitizenPtr> citizens, cTribeToolPtr tool) {
	for (size_t i = 0; i < citizens.size(); i++) {
		UseTool(citizens[i], tool);
	}
}

void cTribeToolStrategy::UseTool(cCreatureCitizenPtr citizen, cTribeToolPtr tool) {
	citizen->DoAction(kCitizenActionGrabTool, tool->ToGameData());
}

/// Tell the selected citizen creatures to use a tool.
void cTribeToolStrategy::SelectedUseTool(cTribeToolPtr tool) {
	if (CanToolBeUsed(tool)) {
		auto selected = GetSelectedMembers();
		if (selected.size() > 0) {
			UseTool(selected, tool);
		}
	}
}