#pragma once

#include <Spore\BasicIncludes.h>
#include "CRE_ViewerAnims.h"

using namespace Simulator;
using namespace Sporepedia;
using namespace Editors;

static bool sporepediapause = false;

// TRG

class HBAssetBrowser
{
public:

	static void Initialize() {
	}

	static void Dispose() {
	}

	static void AttachDetours();

};


// Detour the audio playing func
static_detour(AssetBrowser_PlayAudio_detour, void(uint32_t, Audio::AudioTrack)) {
	void detoured(uint32_t soundID, Audio::AudioTrack track) {
		original_function(soundID, track);
		if (soundID == id("spd_exit") || soundID == id("spd_makenew_click") || soundID == id("spd_verb_edit_click")) {
			if (sporepediapause) {
				sporepediapause = false;
				GameTimeManager.Resume(TimeManagerPause::Gameplay);
				App::ScheduleTask(this, &AssetBrowser_PlayAudio_detour::SendSporepediaClosedMessage, 0.0001f);
			}
		}
	}
	void SendSporepediaClosedMessage() {
		MessageManager.MessageSend(id("SporepediaClosed"), nullptr);
	}
};

// Detour the playanimation ID-picking func
member_detour(AssetBrowser_AnimOverride_detour, Anim::AnimatedCreature, bool(uint32_t, int*)) {
	bool detoured(uint32_t animID, int* pChoice) {

		// Sporepedia viewer
		// TODO: make this work by detecting the sporepedia large card UI instead of game mode.
		// The sporepedia can be opened in any game mode.
		if (GetGameModeID() == kGGEMode) {
			if (ShouldReplaceAnim(animID)) {
				animID = GetRandViewerAnim();
			}
		}

		return original_function(this, animID, pChoice);
	}
};

static_detour(AssetBrowserShow_Detour, void(ShopperRequest&)) {
	void detoured(ShopperRequest & request) {
		original_function(request);
		// pause game. this will be unpaused later by the sound playing
		if (!sporepediapause) {
			sporepediapause = true;
			GameTimeManager.Pause(TimeManagerPause::Gameplay);
		}

	}
};

//-----------------
// ATTACH DETOURS
void HBAssetBrowser::AttachDetours() {
	AssetBrowser_PlayAudio_detour::attach(GetAddress(Audio, PlayAudio));
	AssetBrowser_AnimOverride_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));
	AssetBrowserShow_Detour::attach(GetAddress(ShopperRequest, Show));
}