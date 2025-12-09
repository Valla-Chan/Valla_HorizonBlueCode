#pragma once

#include <Spore\BasicIncludes.h>
#include "Common.h"
// Editor Behaviors
#include "cPaletteUnlock.h"

using namespace Simulator;
using namespace Simulator::Cell;

#define HBEditorsPtr intrusive_ptr<HBEditors>

// Editor

class HBEditors
{
public:

	static void Initialize() {
		auto paletteunlock = new(cPaletteUnlock);
	}

	static void Dispose() {
	}

	static void AttachDetours();
};

//----------------
// Detour Helpers

// Editor parts palette loading func, PaletteUI::Load
static void ED_PaletteUILoad_detour(Palettes::PaletteUI* obj) {
	//PaletteUnlockManager.EditorPaletteUILoad(obj);
}

// PaletteUI::Unload
static void ED_PaletteUIUnLoad_detour(Palettes::PaletteUI* obj) {
}

//----------
// Detours

member_detour(InitializeViewerCamera_detour, ItemViewer, void()) {
	void detoured() {
		original_function(this);
	}
};

// Editor::SetActiveMode
/*
member_detour(EditorSetActiveMode_detour, Editors::cEditor, void(Mode, bool)) {
	void detoured(Mode mode, bool unk1) {
		original_function(this, mode, unk1);
	}
};*/

// Editor::SetEditorModel
member_detour(EditorSetEditorModel_detour, Editors::cEditor, void(EditorModel*)) {
	void detoured(EditorModel* pEditorModel) {
		if (!this->GetEditorModel() || this->GetEditorModel()->mKey.instanceID == 0x0) {
			//PaletteUnlockManager.EditorPaletteUILoadDone(this->mpPartsPaletteUI.get());
		}
		original_function(this, pEditorModel);
	}
};

// CollectableItems::LoadConfig
// TODO: make this load properly for the collections we do want to preserve, and only 0x0 out the ones for CRG, CLG, etc
member_detour(CollectableItemsLoadConfig_detour, cCollectableItems, void(uint32_t, uint32_t, uint32_t)) {
	void detoured(uint32_t configGroupID, uint32_t configInstanceID, uint32_t itemsGroupID) {
		//configGroupID = 0x0;
		//configInstanceID = 0x0;
		// tribal outfit: 0xF71FA311, 0x4A5C4493, 0x40686000 ( created when first loading a new tribal game)
		original_function(this, configGroupID, configInstanceID, itemsGroupID);
	}
};

// PalettePage::ReadItemsModule
member_detour(PalettePageReadItemsModule_detour, Palettes::PaletteItem, void(const ResourceKey&, const ResourceKey&, uint32_t)) {
	void detoured(const ResourceKey& pageName, const ResourceKey& moduleName, uint32_t thumbnailGroupID) {
		//PaletteUnlockManager.ED_ReadItemsModule_detour(moduleName);
		original_function(this, pageName, moduleName, thumbnailGroupID);
	}
};

// temp?
static_detour(EditorRequestSubmit_detour, void(EditorRequest*)) {
	void detoured(EditorRequest* request) {	
		original_function(request);
	}
};

//-----------------
// ATTACH DETOURS
void HBEditors::AttachDetours() {
	InitializeViewerCamera_detour::attach(GetAddress(ItemViewer, InitializeViewerCamera));
	//EditorSetActiveMode_detour::attach(GetAddress(cEditor, SetActiveMode));
	EditorSetEditorModel_detour::attach(GetAddress(cEditor, SetEditorModel));
	CollectableItemsLoadConfig_detour::attach(GetAddress(cCollectableItems, LoadConfig));
	EditorRequestSubmit_detour::attach(GetAddress(EditorRequest, Submit));
	PalettePageReadItemsModule_detour::attach(GetAddress(PalettePage, ReadItemsModule));
}
