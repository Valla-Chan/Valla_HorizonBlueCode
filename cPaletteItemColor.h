#pragma once

#include <Spore\BasicIncludes.h>

#define cPaletteItemColorPtr intrusive_ptr<cPaletteItemColor>

using namespace UTFWin;
using namespace Editors;
using namespace Palettes;

class cPaletteItemColor 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("cPaletteItemColor");
	
	cPaletteItemColor();
	~cPaletteItemColor();

	int AddRef() override { return DefaultRefCounted::AddRef(); }
	int Release() override { return DefaultRefCounted::Release(); }
	void* Cast(uint32_t type) const override;
	
	void InjectListeners();
	void RemoveListeners();

	void UpdateIDColor();
	void ApplyPartColor();
	void ApplyPartColorAllViewers();

	int GetEventFlags() const override;
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;


private:
	const ColorRGB kDefIDColor = ColorRGB(206 / 255.0f, 212 / 255.0f, 175 / 255.0f);

	bool mbUseCrtColor = true;

	hash_map<IWindow*, ItemViewer*> mItemViewers;
	ItemViewer* mpItemViewer = nullptr;
	ColorRGB mIdentityColor = kDefIDColor;
};
