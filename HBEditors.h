#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"
// Editor Behaviors
#include "cPaletteUnlock.h"
#include "cPaletteItemColor.h"

using namespace Simulator;
using namespace Simulator::Cell;

#define HBEditorsPtr intrusive_ptr<HBEditors>

// Editor

cPaletteItemColor* paletteitemcolor;

class HBEditors
{
public:

	static void Initialize() {
		auto paletteunlock = new(cPaletteUnlock);
		paletteitemcolor = new(cPaletteItemColor);
	}

	static void Dispose() {
	}

	static void AttachDetours();
};

//----------------
// Detour Helpers

// Editor parts palette loading func, PaletteUI::Load
static void ED_PaletteUILoad_detour(Palettes::PaletteUI* obj) {
	PaletteUnlockManager.EditorPaletteUILoad(obj);
	paletteitemcolor->InjectListeners();
}

// PaletteUI::Unload
static void ED_PaletteUIUnLoad_detour(Palettes::PaletteUI* obj) {
	paletteitemcolor->RemoveListeners();
}

// Editor::SetActiveMode
static void ED_EditorSetActiveMode_detour(Editors::cEditor* obj, Mode mode) {
	if (mode == Mode::BuildMode) {
		paletteitemcolor->ApplyPartColorAllViewers();
	}
}

//----------
// Detours

member_detour(InitializeViewerCamera_detour, ItemViewer, void()) {
	void detoured() {
		original_function(this);
		paletteitemcolor->ApplyPartColor();
	}
};

// Editor::SetActiveMode
member_detour(EditorSetActiveMode_detour, Editors::cEditor, void(Mode, bool)) {
	void detoured(Mode mode, bool unk1) {
		if (mode == Mode::BuildMode) {
			paletteitemcolor->ApplyPartColorAllViewers();
		}
		original_function(this, mode, unk1);
	}
};

// Editor::SetEditorModel
member_detour(EditorSetEditorModel_detour, Editors::cEditor, void(EditorModel*)) {
	void detoured(EditorModel* pEditorModel) {
		paletteitemcolor->ApplyPartColorAllViewers();
		if (!this->GetEditorModel() || this->GetEditorModel()->mKey.instanceID == 0x0) {
			PaletteUnlockManager.EditorPaletteUILoadDone(this->mpPartsPaletteUI.get());
		}
		original_function(this, pEditorModel);
	}
};

member_detour(CollectableItemsLoadConfig_detour, cCollectableItems, void(uint32_t, uint32_t, uint32_t)) {
	void detoured(uint32_t configGroupID, uint32_t configInstanceID, uint32_t itemsGroupID) {
		configGroupID = 0x0;
		configInstanceID = 0x0;
		original_function(this, configGroupID, configInstanceID, itemsGroupID);
	}
};

static_detour(EditorRequestSubmit_detour, void(EditorRequest*)) {
	void detoured(EditorRequest* request) {
		//GetPlayer()->mpCRGItems->LoadConfig(0x0, 0x0, 0x0);
		//GetPlayer()->mpCRGItems->mItemStatusInfos.clear();
		//GetPlayer()->mpCRGItems->mUnlockableRows.clear();
		//GetPlayer()->mpCRGItems->mUnlockableItems.clear();

		/*
		auto crgitems = GetPlayer()->mpCRGItems;

		eastl::sp_fixed_hash_map<cCollectableItemID, uint8_t, 256> mitemStatusInfosNew;

		auto iterator = GetPlayer()->mpCRGItems->mItemStatusInfos.begin();
		while (iterator != GetPlayer()->mpCRGItems->mItemStatusInfos.end()) {
			//mitemStatusInfosNew.emplace(iterator->first, 1);
			iterator.increment();
		}
		GetPlayer()->mpCRGItems->mItemStatusInfos.reset_lose_memory();

		auto iterator2 = mitemStatusInfosNew.begin();
		while (iterator2 != mitemStatusInfosNew.end()) {
			//GetPlayer()->mpCRGItems->mItemStatusInfos.emplace(iterator2->first, 1);
			GetPlayer()->mpCRGItems->mItemStatusInfos.insert(make_pair(iterator2->first, 1));
			iterator2.increment();
		}*/
		
		original_function(request);
	}
};

//-----------------
// ATTACH DETOURS
void HBEditors::AttachDetours() {
	InitializeViewerCamera_detour::attach(GetAddress(ItemViewer, InitializeViewerCamera));
	EditorSetActiveMode_detour::attach(GetAddress(cEditor, SetActiveMode));
	EditorSetEditorModel_detour::attach(GetAddress(cEditor, SetEditorModel));
	CollectableItemsLoadConfig_detour::attach(GetAddress(cCollectableItems, LoadConfig));
	EditorRequestSubmit_detour::attach(GetAddress(EditorRequest, Submit));
}
