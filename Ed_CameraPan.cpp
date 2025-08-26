#include "stdafx.h"
#include "Ed_CameraPan.h"

Ed_CameraPan::Ed_CameraPan()
{
	//WindowManager.GetMainWindow()->AddWinProc(this);
}


Ed_CameraPan::~Ed_CameraPan()
{
}

// For internal use, do not modify.
int Ed_CameraPan::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int Ed_CameraPan::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* Ed_CameraPan::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(Ed_CameraPan);
	return nullptr;
}

void Ed_CameraPan::AttachDetours() {
	Editor_SetActiveMode_detour::attach(GetAddress(cEditor, SetActiveMode));
	Editor_SetEditorModel_detour::attach(GetAddress(cEditor, SetEditorModel));

	EditorCamera_OnMouseMove_detour::attach(GetAddress(EditorCamera, OnMouseMove));
	EditorCamera_OnMouseWheel_detour::attach(GetAddress(EditorCamera, OnMouseWheel));
	EditorCamera_Update_detour::attach(GetAddress(EditorCamera, Update));
	EditorCamera_OnEnter_detour::attach(GetAddress(EditorCamera, OnEnter));
	EditorCamera_OnKeyDown_detour::attach(GetAddress(EditorCamera, OnKeyDown));
}



int Ed_CameraPan::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

bool Ed_CameraPan::HandleUIMessage(IWindow* window, const Message& message)
{
	if (message.IsType(kMsgKeyPress)) {

		//if (!(Editor.IsActive() && Editor.IsMode(Editors::Mode::BuildMode))) { return false; }

		//switch (message.Key.vkey) {
		//	case 53: // Numpad 5
		//		editorcamera_xoffset = 0; break;
			//case VK_DECIMAL:
			//	editorcamera_xoffset = 0; break;
		//}
	}
	return false;
}
