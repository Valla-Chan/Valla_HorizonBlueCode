#include "stdafx.h"
#include "cPaletteItemColor.h"

cPaletteItemColor::cPaletteItemColor()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
}


cPaletteItemColor::~cPaletteItemColor()
{
}

void* cPaletteItemColor::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(cPaletteItemColor);
	return nullptr;
}

int cPaletteItemColor::GetEventFlags() const {
	return UTFWin::kEventRefresh;
}

bool cPaletteItemColor::HandleUIMessage(IWindow* pWindow, const Message& message) {
	if (message.IsType(kMsgMouseEnter)) {
		if (mItemViewers[pWindow]) {
			mpItemViewer = mItemViewers[pWindow];
		}
	}
	return false;
}

// Update the mIdentityColor to the current color in the editor, or to the default color if not using the editor color.
void cPaletteItemColor::UpdateIDColor() {
	if (mbUseCrtColor && Editor.GetEditorModel() && Editor.GetSkin()) {
		mIdentityColor = Editor.GetEditorModel()->GetColor(0);
	}
	else {
		mIdentityColor = kDefIDColor;
	}
}

// Apply color to the currently hovered part.
// NOTE: this only applies once!
// For mbUseCrtColor, we may want to blend between ID color and the default color according to the current model zoom
// TODO: for some reason the mpItemViewer->mpModels are being preserved after editor shutdown. try to fix this. 
void cPaletteItemColor::ApplyPartColor() {
	if (mpItemViewer && mpItemViewer->mpModel && mpItemViewer->mpModel->mpWorld) {
		UpdateIDColor();
		mpItemViewer->mpModel->SetColor(ColorRGBA(mIdentityColor, 1.0f));
	}
}

// Apply color to all pre-existing part viewer models
// Only useful when mbUseCrtColor is true. Called when switching editor modes.
void cPaletteItemColor::ApplyPartColorAllViewers() {
	if (!mbUseCrtColor) return;
	UpdateIDColor();
	for (auto item : mItemViewers) {
		if (item.second && item.second->mpModel) {
			item.second->mpModel->SetColor(ColorRGBA(mIdentityColor, 1.0f));
		}
	}
}


void cPaletteItemColor::InjectListeners() {

	PaletteUIPtr palette = GetEditor()->mpPartsPaletteUI;
	if (!palette) { return; }

	RemoveListeners();
	for (PaletteCategoryUIPtr catUI : palette->mCategories) {
		PaletteSubcategoriesUIPtr subCatUIs = catUI->mpSubcategoriesUI;

		// subcategories present
		if (subCatUIs) {
			for (PaletteCategoryUIPtr subCatUI : subCatUIs->mCategoryUIs) {
				for (auto pageUI : subCatUI->mPageUIs) {
					// standard editor/planner
					if (pageUI.page->mStandardItems.size() > 0) {
						for (StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
							itemUI->mpWindow->AddWinProc(this);
							mItemViewers[itemUI->mpWindow.get()] = itemUI->mpViewer.get();
						}
					}
				}
			}
		}
		// simple category
		else {
			for (auto pageUI : catUI->mPageUIs) {
				if (pageUI.page->mStandardItems.size() > 0) {
					for (StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
						itemUI->mpWindow->AddWinProc(this);
						mItemViewers[itemUI->mpWindow.get()] = itemUI->mpViewer.get();
					}
				}
			}
		}

	}
}



void cPaletteItemColor::RemoveListeners() {

	PaletteUIPtr palette = GetEditor()->mpPartsPaletteUI;
	if (!palette) { return; }

	for (auto catUI : palette->mCategories) {
		auto subCatUIs = catUI->mpSubcategoriesUI;

		// subcategories present
		if (subCatUIs) {
			for (PaletteCategoryUIPtr subCatUI : subCatUIs->mCategoryUIs) {
				for (auto pageUI : subCatUI->mPageUIs) {
					// standard editor/planner
					if (pageUI.page->mStandardItems.size() > 0) {
						for (StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
							itemUI->mpWindow->RemoveWinProc(this);
						}
					}
				}
			}
		}
		// simple category
		else {
			for (auto pageUI : catUI->mPageUIs) {
				for (StandardItemUIPtr itemUI : pageUI.page->mStandardItems) {
					if (itemUI && itemUI->mpWindow) {
						itemUI->mpWindow->RemoveWinProc(this);
					}
				}
			}
		}
	}
	mItemViewers.clear();
	mpItemViewer = nullptr;
}