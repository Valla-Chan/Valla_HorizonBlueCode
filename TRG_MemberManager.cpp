#include "stdafx.h"
#include "TRG_MemberManager.h"

/// AUTOGENERATED METHODS ///

int TRG_MemberManager::AddRef() {
	return Simulator::cStrategy::AddRef();
}
int TRG_MemberManager::Release() {
	return Simulator::cStrategy::Release();
}

const char* TRG_MemberManager::GetName() const {
	return "Valla_CreatureOverhaul::TRG_MemberManager";
}

bool TRG_MemberManager::Write(Simulator::ISerializerStream* stream)
{
	return Simulator::ClassSerializer(this, ATTRIBUTES).Write(stream);
}
bool TRG_MemberManager::Read(Simulator::ISerializerStream* stream)
{
	return Simulator::ClassSerializer(this, ATTRIBUTES).Read(stream);
}
//------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------

Simulator::Attribute TRG_MemberManager::ATTRIBUTES[] = {
	// Add more attributes here
	//SimAttribute(TRG_MemberManager, mCreaturePersonalities, 1),
	// This one must always be at the end
	Simulator::Attribute()
};

void TRG_MemberManager::Initialize() {
}

void TRG_MemberManager::Dispose() {
	
}

void TRG_MemberManager::Update(int deltaTime, int deltaGameTime) {
	
}