#include "stdafx.h"
#include "TRG_TribeHutManager.h"
#include <Spore\Editors\BakeManager.h>

TRG_TribeHutManager::TRG_TribeHutManager()
{
}

TRG_TribeHutManager::~TRG_TribeHutManager()
{
}

int TRG_TribeHutManager::AddRef() {
	return Simulator::cStrategy::AddRef();
}
int TRG_TribeHutManager::Release() {
	return Simulator::cStrategy::Release();
}

const char* TRG_TribeHutManager::GetName() const {
	return "Valla_CreatureOverhaul::TRG_TribeHutManager";
}

bool TRG_TribeHutManager::Write(Simulator::ISerializerStream* stream)
{
	UpdateStoredTribeNames();
	SporeDebugPrint("TRG_TribeHutManager is writing...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool TRG_TribeHutManager::Read(Simulator::ISerializerStream* stream)
{
	SporeDebugPrint("TRG_TribeHutManager is reading...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
}

void TRG_TribeHutManager::Update(int deltaTime, int deltaGameTime) {

}

//-----------------------------------------------------------------------------------

void TRG_TribeHutManager::Initialize() {
	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, id("UpdateHomes"));
	MessageManager.AddListener(this, id("UpdateHut"));

	//MessageManager.AddListener(this, id("TribeNameUpdated")); // Calls UpdateStoredTribeNames()
	//MessageManager.AddListener(this, id("UpdateTribeNames")); // Calls UpdateTribeNamesFromStored()
}

void TRG_TribeHutManager::Dispose() {
}

//-------------------------------------------
// Store and load tribe names manually

void TRG_TribeHutManager::UpdateStoredTribeNames() {
	mTribeNames.clear();
	auto tribes = GetDataByCast<cTribe>();
	for (size_t i = 0; i < tribes.size(); i++ ) {
		mTribeNames[i] = tribes[i]->GetCommunityName();
	}
}

void TRG_TribeHutManager::UpdateTribeNamesFromStored() {
	auto tribes = GetDataByCast<cTribe>();
	for (size_t i = 0; i < tribes.size(); i++) {
		if (mTribeNames.size() > i) {
			tribes[i]->SetName(mTribeNames[i].c_str());
		}
	}
}

//-------------------------------------------

void TRG_TribeHutManager::OpenHutShopper() {
	auto request = Sporepedia::ShopperRequest(this);
	request.shopperID = id("FixedObjectHutShopper");
	//request.pEditorRequest
	request.Show(request);
}

void TRG_TribeHutManager::OnShopperAccept(const ResourceKey& selection)
{
	if (selection == ResourceKey()) { return; }

	PropertyListPtr propList;
	if (!PropManager.GetPropertyList(selection.instanceID, selection.groupID, propList))
	{
		BakeManager.Bake(selection, NULL);
	}
	// store model
	if (mHutType == Main) { mHutResMain = selection; }
	else { mHutResHome = selection; }
	
	UpdateHutIcon(mHutType);
	UpdateHutModels(mHutType);
	mHutType = -1;
}

void TRG_TribeHutManager::OnModeEntered(uint32_t previousModeID, uint32_t newModeID) {
	if (newModeID == kGameTribe) {
		UpdateTribeNamesFromStored();
		SporeDebugPrint("Updating tribe names from stored vars.");
	}
}

// TODO: detect when the planner is opened/hut page is loaded, and update the icon.

//-------------------------------------------

// Gets the hut icon resource path from the premade / user-generated hut
ResourceKey TRG_TribeHutManager::GetHutIconResource(ResourceKey selection) const {
	// If the model is a premade prop, look for its thumbnail in the correct folder.
	if (selection.typeID == Names::prop) {
		selection.groupID = id("PlannerThumbnails");
	}
	return selection;
}

// Gets the icon texture from the icon resource
Graphics::Texture* TRG_TribeHutManager::GetHutIcon(int hutType) const {
	ResourceKey hutKey;
	if (hutType == Main) {
		if (mHutResMain == ResourceKey()) { return nullptr; }
		hutKey = GetHutIconResource(mHutResMain);
	}
	if (hutType == Home) {
		if (mHutResHome == ResourceKey()) { return nullptr; }
		hutKey = GetHutIconResource(mHutResHome);
	}

	return TextureManager.GetTexture(hutKey, Graphics::TextureFlags::kTextureFlagForceLoad);
}


void TRG_TribeHutManager::UpdateHutIcon(int hutType) {
	ResourceKey hutResource;
	//// Chieftain Hut
	if (hutType == Main) {
		if (mHutResMain == ResourceKey()) { return; }
		hutResource = mHutResMain;
		auto window = WindowManager.GetMainWindow();
		auto imagewindow = window->FindWindowByID(0x7D081C1F);

		if (imagewindow) {
			auto icon = GetHutIconResource(hutResource);
			if (icon == ResourceKey()) { return; }
			object_cast<IImageDrawable>(imagewindow->GetDrawable())->SetImageForWindow(imagewindow, icon);
		}

	}
	//// Lodgings
	else {
		if (mHutResHome == ResourceKey()) { return; }
		hutResource = mHutResHome;
	}

}

void TRG_TribeHutManager::UpdateHutModels(int hutType) {
	auto tribe = GameNounManager.GetPlayerTribe();
	if (!tribe) { return; }

	//// Chieftain Hut
	if (hutType == Main) {
		// cancel if the model key or hut doesnt exist
		auto hut = tribe->mpHut;
		if (mHutResMain == ResourceKey() || !hut) { return; }

		hut->SetModelKey(mHutResMain);
		hut->mUndamagedModel = mHutResMain;
		hut->mDestructModelLo = mHutResMain;
		hut->mDestructModelMd = mHutResMain;
		hut->mDestructModelHi = mHutResMain;
		hut->SetScale(3.0f);
	}

	//// Lodgings
	else {
		// cancel if the model key doesnt exist
		if (mHutResHome == ResourceKey()) { return; }

		// TODO: somewhere probably in DLLmain, we will need to pull from mHutResHome
		// and use that for the model keys returned by the tribe tool data for the home huts.
		// Also figure out a good way to set the palette model.

		// loop thru all tribe tools
		for (auto item : tribe->GetTools()) {
			// if the tool is a home hut
			if (item->GetToolType() >= HomeStart && item->GetToolType() <= HomeEnd) {
				// TODO: do not do this if the hut isnt built yet!
				item->SetModelKey(mHutResHome);
				item->SetScale(2.5f);
			}
		}
	}
}

//-------------------------------------------

bool TRG_TribeHutManager::HandleMessage(uint32_t messageID, void* msg)
{
	if (!IsTribeGame()) { return false; }

	if (messageID == id("UpdateHut")) {
		UpdateHutModels(0);
	}
	else if (id("UpdateHomes")) {
		UpdateHutModels(1);
	}
	return false;
}

int TRG_TribeHutManager::GetEventFlags() const
{
	return kEventFlagBasicInput;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool TRG_TribeHutManager::HandleUIMessage(IWindow* window, const Message& message)
{
	if (!IsTribeGame()) { return false; }

	if (message.IsType(kMsgButtonClick)) {
		auto id = message.source->GetControlID();

		// Tribe Hut Buttons
		if (id >= 0x04A1CB6B && id <= 0x04A1CB6D) {
			// Set hut type to Main or Home depending on button parent ID
			mHutType = message.source->GetParent()->GetControlID() == 0x04A100B1;

			// pressed Sporepedia browse
			if (id == 0x04A1CB6D) {
				OpenHutShopper();
			}
			// pressed Edit
			if (id == 0x04A1CB6C) {
				OpenHutShopper();
			}
			// pressed Make New
			if (id == 0x04A1CB6B) {
				OpenHutShopper();
			}
		}

	}
	return false;
}

//-------------------------------------------

Simulator::Attribute TRG_TribeHutManager::ATTRIBUTES[] = {
	// Add more attributes here
	SimAttribute(TRG_TribeHutManager,mHutResMain,0),
	SimAttribute(TRG_TribeHutManager,mHutResHome,1),
	SimAttribute(TRG_TribeHutManager,mTribeNames,2),
	// This one must always be at the end
	Simulator::Attribute()
};

// You can extend this function to return any other types your class implements.
void* TRG_TribeHutManager::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(TRG_TribeHutManager);
	return nullptr;
}
