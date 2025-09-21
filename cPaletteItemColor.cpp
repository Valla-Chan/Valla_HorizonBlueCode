#include "stdafx.h"
#include "cPaletteItemColor.h"

cPaletteItemColor::cPaletteItemColor()
{
	WindowManager.GetMainWindow()->AddWinProc(this);
	App::AddUpdateFunction(this);
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

// Blend between default color and creature color according to the current model zoom
void cPaletteItemColor::Update()
{
	/*
	if (mbUseCrtColor && Simulator::IsEditorMode() && Editor.GetEditorModel() && Editor.GetSkin() &&
	mpItemViewer && mpItemViewer->mpModel && mpItemViewer->mpModel->mpWorld) {
		float zoomscale = abs(mpItemViewer->mZoom / 2.0f);
		if (mpItemViewer->mFinalZoom == 1 && mpItemViewer->mZoom == 1) {
			zoomscale = 0;
		}
		SporeDebugPrint("values: initial %f  final %f  current %f", mpItemViewer->mInitialZoom, mpItemViewer->mFinalZoom, mpItemViewer->mZoom);
		SporeDebugPrint("prescaled: %f", zoomscale);
		zoomscale = zoomscale * zoomscale * zoomscale;
		SporeDebugPrint("scaled: %f", zoomscale);
		ColorRGB finalcolor = Editor.GetEditorModel()->GetColor(0);
		ColorRGB color = {
			lerp(kDefIDColor.r, finalcolor.r, zoomscale),
			lerp(kDefIDColor.g, finalcolor.g, zoomscale),
			lerp(kDefIDColor.b, finalcolor.b, zoomscale),
		};
		mpItemViewer->mpModel->SetColor(ColorRGBA(color, 1.0f));
	}*/
}

int cPaletteItemColor::GetEventFlags() const {
	return UTFWin::kEventRefresh | kEventFlagUpdate;
}

bool cPaletteItemColor::HandleUIMessage(IWindow* pWindow, const Message& message) {
	if (message.IsType(kMsgMouseEnter)) {
		if (mItemViewers[pWindow]) {
			mpItemViewer = mItemViewers[pWindow];
		}
	}
	else if (message.IsType(kMsgUpdate)) {
		auto viewer = mItemViewers[pWindow];
		if (mbUseCrtColor && Simulator::IsEditorMode() && Editor.GetEditorModel() && Editor.GetSkin() &&
			viewer && viewer->mpModel && viewer->mpModel->mpWorld) {
			float zoomscale = abs(viewer->mZoom / 2.0f);
			if (viewer->mFinalZoom == 1 && viewer->mZoom == 1) {
				zoomscale = 0;
			}
			zoomscale = pow(zoomscale, 3.0f) / 1.2f;
			ColorRGB finalcolor = Editor.GetEditorModel()->GetColor(0);
			ColorRGB color = {
				lerp(kDefIDColor.r, finalcolor.r, zoomscale),
				lerp(kDefIDColor.g, finalcolor.g, zoomscale),
				lerp(kDefIDColor.b, finalcolor.b, zoomscale),
			};
			viewer->mpModel->SetColor(ColorRGBA(color, 1.0f));
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
// For mbUseCrtColor, disable this because we need to blend the colors
void cPaletteItemColor::ApplyPartColor() {
	if (!mbUseCrtColor && mpItemViewer && mpItemViewer->mpModel && mpItemViewer->mpModel->mpWorld) {
		UpdateIDColor();
		mpItemViewer->mpModel->SetColor(ColorRGBA(mIdentityColor, 1.0f));
	}
}
/*
// Apply color to all pre-existing part viewer models
// Only useful when mbUseCrtColor is true. Called when switching editor modes.
void cPaletteItemColor::ApplyPartColorAllViewers() {
	return; // DISABLE THIS CODE CURRENTLY
	if (!mbUseCrtColor) return;
	UpdateIDColor();
	for (auto item : mItemViewers) {
		if (item.second && item.second->mpModel) {
			item.second->mpModel->SetColor(ColorRGBA(mIdentityColor, 1.0f));
		}
	}
}
*/


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