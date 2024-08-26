#include "stdafx.h"
#include "SetGlideSpeed.h"

SetGlideSpeed::SetGlideSpeed()
{
}


SetGlideSpeed::~SetGlideSpeed()
{
}


// TODO: this WORKS, but only when re-entering the stage.
void SetGlideSpeed::ParseLine(const ArgScript::Line& line)
{
	App::Property* prop;
	PropertyListPtr mpPropList;
	if (PropManager.GetPropertyList(id("CreatureGameTuning"), id("CreatureGame"), mpPropList)) {
		mpPropList->GetProperty(id("creatureGlideSpeed"), prop);
		prop->SetValueFloat(500);
	}
	
}

const char* SetGlideSpeed::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat does something.";
	}
	else {
		return "SetGlideSpeed: Elaborate description of what this cheat does.";
	}
}
