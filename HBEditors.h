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
		//if (mode == Mode::BuildMode) {
			//paletteitemcolor->ApplyPartColorAllViewers();
		//}
		original_function(this, mode, unk1);
	}
};

// Editor::SetEditorModel
member_detour(EditorSetEditorModel_detour, Editors::cEditor, void(EditorModel*)) {
	void detoured(EditorModel* pEditorModel) {
		//paletteitemcolor->ApplyPartColorAllViewers();
		if (!this->GetEditorModel() || this->GetEditorModel()->mKey.instanceID == 0x0) {
			PaletteUnlockManager.EditorPaletteUILoadDone(this->mpPartsPaletteUI.get());
		}
		original_function(this, pEditorModel);
	}
};

// CollectableItems::LoadConfig
// TODO: make this load properly for the collections we do want to preserve, and only 0x0 out the ones for CRG, CLG, etc
member_detour(CollectableItemsLoadConfig_detour, cCollectableItems, void(uint32_t, uint32_t, uint32_t)) {
	void detoured(uint32_t configGroupID, uint32_t configInstanceID, uint32_t itemsGroupID) {
		configGroupID = 0x0;
		configInstanceID = 0x0;
		original_function(this, configGroupID, configInstanceID, itemsGroupID);
	}
};

// PalettePage::ReadItemsModule
member_detour(PalettePageReadItemsModule_detour, Palettes::PaletteItem, void(const ResourceKey&, const ResourceKey&, uint32_t)) {
	void detoured(const ResourceKey& pageName, const ResourceKey& moduleName, uint32_t thumbnailGroupID) {
		PaletteUnlockManager.ED_ReadItemsModule_detour(moduleName);
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
	EditorSetActiveMode_detour::attach(GetAddress(cEditor, SetActiveMode));
	EditorSetEditorModel_detour::attach(GetAddress(cEditor, SetEditorModel));
	CollectableItemsLoadConfig_detour::attach(GetAddress(cCollectableItems, LoadConfig));
	EditorRequestSubmit_detour::attach(GetAddress(EditorRequest, Submit));
	PalettePageReadItemsModule_detour::attach(GetAddress(PalettePage, ReadItemsModule));
}
