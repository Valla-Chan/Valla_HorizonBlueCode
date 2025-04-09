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
	SporeDebugPrint("TRG_TribeHutManager is writing...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool TRG_TribeHutManager::Read(Simulator::ISerializerStream* stream)
{
	has_pulled_tribenames = false;
	if (!mTribeName.empty()) {
		mTribeName.clear();
	}
	if (mHutResMain.instanceID != 0x0 || mHutResHome.instanceID != 0x0) {
		mHutResMain = {};
		mHutResHome = {};
	}
	SporeDebugPrint("TRG_TribeHutManager is reading...");
	return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
}

void TRG_TribeHutManager::Update(int deltaTime, int deltaGameTime) {
	if (GameViewManager.GetHoveredObject()) {
		auto hut = object_cast<cTribeHut>(GameViewManager.GetHoveredObject());
		if (hut && hut->mHealthPoints > 0 && hut->GetTribe() && !hut->GetTribe()->IsPlayerOwned()) {
			UI::SimulatorRollover::ShowRollover(hut);
		}
		
	}
}

//-----------------------------------------------------------------------------------

void TRG_TribeHutManager::Initialize() {
	WindowManager.GetMainWindow()->AddWinProc(this);
	MessageManager.AddListener(this, id("UpdateHomes"));
	MessageManager.AddListener(this, id("UpdateHut"));

	MessageManager.AddListener(this, id("TribeNameUpdated")); // Calls UpdateStoredTribeNames()
	//MessageManager.AddListener(this, id("UpdateTribeNames")); // Calls UpdateTribeNamesFromStored()
}

void TRG_TribeHutManager::Dispose() {
}

//-------------------------------------------
// Store and load tribe names manually

// Add rename UI to planner
void TRG_TribeHutManager::AddTribeRenameUI(bool paletteLoaded) {
	if (paletteLoaded && !Editors::GetEditor()->mpEditorNamePanel) {
		auto window = WindowManager.GetMainWindow();
		auto nameslot = window->FindWindowByID(0x272EB68E);

		// remove any existing naming UIs
		auto nameWindowOld = nameslot->FindWindowByID(0x272EB68E);
		if (nameWindowOld) {
			nameslot->DisposeWindowFamily(nameWindowOld);
		}

		// create new naming UI
		auto namepanel = Editors::GetEditor()->mpEditorNamePanel;
		namepanel = new(UI::EditorNamePanel);
		namepanel->Initialize(GameNounManager.GetPlayerTribe(), nameslot, id("EditorNameDescribe-NoTag"), true, uint32_t(SpaceNamesType::kTribe));

		MessageManager.MessageSend(id("TribeNameUpdated"), nullptr);
	}
	else if (!paletteLoaded) {
		MessageManager.MessageSend(id("TribeNameUpdated"), nullptr);
	}
}



// recalculate names of all tribes
void TRG_TribeHutManager::UpdateNPCTribeNames() {
	auto tribes = GetData<cTribe>();
	for (auto tribe : tribes) {
		// do not run on player tribe
		if (tribe != GameNounManager.GetPlayerTribe()) {
			SetTribeName(tribe);
		}
	}
}

void TRG_TribeHutManager::SetupNewTribe(cTribePtr tribe) {
	SetTribeName(tribe);
	//SetTribeColor(tribe);

	// Set Tribal hut
	int archetype = tribe->mTribeArchetype;
	int tier = 2; // TODO: figure out what defines the hut tier in NPC tribes.
	uint32_t style = Canvas;
	ResourceKey hutstyle;

	// Set tribe hut from archetype
	if (!tribe->IsPlayerOwned() && archetype > -1) {
		switch (archetype) {
			case 7: style = Omnivore; break;
			case 4: style = Carnivore; break;
		}
		hutstyle = GetHutStyle(style, tier);
		if (hutstyle.instanceID != 0x0) {
			SetTribeHutStyle(tribe, hutstyle);
		}
	}
}

// 'hutstyle' is a keys property in the hutstyles manifest file that contains hut tiers.
// tier is typically 1-3
ResourceKey TRG_TribeHutManager::GetHutStyle(uint32_t hutstyle, int tier) const {
	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(hutstyles_manifest.instanceID, hutstyles_manifest.groupID, mpPropList))
	{
		size_t keycount;
		ResourceKey* keys;
		if (App::Property::GetArrayKey(mpPropList.get(), hutstyle, keycount, keys)) {
			if (keycount > 0) {
				tier = clamp(tier-1, 0, int(keycount-1));
				return keys[tier];
			}
		}
	}
	return ResourceKey();
}

// Apply a hutstyle to a tribe
void TRG_TribeHutManager::SetTribeHutStyle(cTribePtr tribe, ResourceKey &hutstyle) {
	ResourceKey hutDmgHi = hutstyle;
	ResourceKey hutDmgMd = hutstyle;
	ResourceKey hutDmgLo = hutstyle;
	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(hutstyle.instanceID, hutstyle.groupID, mpPropList))
	{
		App::Property::GetKey(mpPropList.get(), 0x492D3892, hutDmgHi);
		App::Property::GetKey(mpPropList.get(), 0x492D3893, hutDmgMd);
		App::Property::GetKey(mpPropList.get(), 0x492D3894, hutDmgLo);

		//tribe->GetHut()->SetModelKey(hutstyle);
		tribe->GetHut()->mUndamagedModel = hutstyle;
		tribe->GetHut()->mDestructModelHi = hutDmgHi;
		tribe->GetHut()->mDestructModelMd = hutDmgMd;
		tribe->GetHut()->mDestructModelLo = hutDmgLo;
	}
	else { return; }
	
}

// Set NPC tribe name
void TRG_TribeHutManager::SetTribeName(cTribePtr tribe) {
	int archetype = tribe->mTribeArchetype;
	LocalizedString tribeName;

	// Set tribe name from archetype
	if (!tribe->IsPlayerOwned() && archetype > -1) {
		// Neutral
		if (archetype == 15 || archetype == 12) {
			tribeName = LocalizedString(id("TribeArchetypes"), 0x00000000);
		}
		// Orange
		else if (archetype == 7) {
			tribeName = LocalizedString(id("TribeArchetypes"), 0x00000002);
		}
		// Red
		else if (archetype == 4) {
			tribeName = LocalizedString(id("TribeArchetypes"), 0x00000003);
		}
		// Default
		else {
			tribeName = LocalizedString(id("TribeArchetypes"), 0x00000000);
		}


		auto text = tribeName.GetText();
		if (text) {
			// get the species profile from the key
			cSpeciesProfile* speciesProfile = SpeciesManager.GetSpeciesProfile(tribe->mSpeciesKeys[0]);

			// get the text from the species profile
			string16 speciesText;
			speciesProfile->GetSpeciesName(speciesText);

			// combine the species name with the village locale
			speciesText.append(u" ");
			speciesText.append(string16(text));

			tribe->SetName(speciesText.c_str());
		}
	}
	
}

// Set NPC tribe color to some of the new colors
// TODO: this is woefully unfinished!!
void TRG_TribeHutManager::SetTribeColor(cTribePtr tribe) {
	auto colorID = tribe->mIDColorID;

	// Only change the color if it is not one of the player/neutral colors
	if (colorID != uint32_t(IdentityColors::Player) && colorID != uint32_t(IdentityColors::Neutral)) {
		// DEBUG
		//tribe->mIDColorID = 0x053dbca1;
	}
	
}


// store the player tribe's name
void TRG_TribeHutManager::UpdateStoredTribeNames() {
	auto tribe = GameNounManager.GetPlayerTribe();
	if (tribe) {
		mTribeName = tribe->GetCommunityName();
	}
}

void TRG_TribeHutManager::UpdateTribeNamesFromStored() {
	if (has_pulled_tribenames) { return; }

	//SporeDebugPrint("Updating tribe names from stored vars.");
	UpdateNPCTribeNames();
	// update player tribe name
	auto tribe = GameNounManager.GetPlayerTribe();
	if (tribe && mTribeName.length() > 0) {
		tribe->SetName(mTribeName.c_str());
	}
	has_pulled_tribenames = true;
	
}

//-------------------------------------------
// Editor / Sporepedia

void TRG_TribeHutManager::OnModeEntered(uint32_t previousModeID, uint32_t newModeID) {
	cStrategy::OnModeEntered(previousModeID, newModeID);
	// Entered tribe game
	if (newModeID != previousModeID && newModeID == kGameTribe) {
		has_pulled_tribenames = false;
		Simulator::ScheduleTask(this, &TRG_TribeHutManager::UpdateTribeNamesFromStored, 0.01f);
	}
}

void TRG_TribeHutManager::OpenHutShopper() {
	auto request = Sporepedia::ShopperRequest(this);
	request.shopperID = id("FixedObjectHutShopper");
	request.Show(request);
}

void TRG_TribeHutManager::OpenStaffShopper() {
	auto request = Sporepedia::ShopperRequest(this);
	request.shopperID = id("FixedObjectStaffShopper");
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
	switch (mHutType) {
		case Main:
			mHutResMain = selection; break;
		case Home:
			mHutResHome = selection; break;
		}
	
	UpdateHutIcon(mHutType);
	UpdateHutModels(mHutType);
	mHutType = -1;
}



// Detect when the planner is opened/hut page is loaded, and update the icon.

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

		// TODO: this is not correct!
		// If there is no resource, it should default back to hut image of the assigned diet type.
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
		UpdateHutIcon(0);
	}
	else if (messageID == id("UpdateHomes")) {
		UpdateHutModels(1);
		UpdateHutIcon(1);
	}
	//else if (messageID == id("UpdateTribeNames")) {
	//	UpdateTribeNamesFromStored();
	//}
	else if (messageID == id("TribeNameUpdated")) {
		UpdateStoredTribeNames();
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
		else if (id == id >= 0x04AAFF6B && id <= 0x04AAFF6D) {
			OpenStaffShopper();
		}
		// Planner Accept Button
		else if (id == 0x1036A039) {
			UpdateStoredTribeNames();
		}


	}
	return false;
}

//-------------------------------------------

Simulator::Attribute TRG_TribeHutManager::ATTRIBUTES[] = {
	// Add more attributes here
	SimAttribute(TRG_TribeHutManager,mHutResMain,1),
	SimAttribute(TRG_TribeHutManager,mHutResHome,2),
	SimAttribute(TRG_TribeHutManager,mTribeName,3),
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
