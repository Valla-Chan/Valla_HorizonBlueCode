#include "stdafx.h"
#include "cCreatureVector.h"

cCreatureVector::cCreatureVector()
{
}

cCreatureVector::cCreatureVector(vector<cCreatureBasePtr> vector)
{
	mpCreatureVec = vector;
}

cCreatureVector::cCreatureVector(vector<cCreatureAnimalPtr> vector)
{
	SetVector(vector);
}
cCreatureVector::cCreatureVector(vector<cCreatureCitizenPtr> vector)
{
	SetVector(vector);
}

cCreatureVector::~cCreatureVector()
{
}

// For internal use, do not modify.
int cCreatureVector::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int cCreatureVector::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* cCreatureVector::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(cCreatureVector);
	return nullptr;
}

//--------------------------------------------------------------------------

vector<cCreatureBasePtr> cCreatureVector::GetVector() {
	return mpCreatureVec;
}

void cCreatureVector::Clear() {
	mpCreatureVec.clear();
}

int cCreatureVector::Find(cCreatureBasePtr creature) const {
	for (size_t i = 0; i < mpCreatureVec.size(); i++) {
		if (mpCreatureVec[i] == creature) {
			return i;
		}
	}
	return -1;
}

bool cCreatureVector::Has(cCreatureBasePtr creature) const {
	return Find(creature) > -1;
}

bool cCreatureVector::Remove(int index) {
	if (index == -1) {
		index = mpCreatureVec.size() - 1;
	}
	if (index > -1 && index < int(mpCreatureVec.size()) && mpCreatureVec[index]) {
		mpCreatureVec[index] = nullptr;
		// If last object, decrease array size.
		if (index == mpCreatureVec.size() - 1) {
			mpCreatureVec.resize(mpCreatureVec.size() - 1);
		}
		return true;
	}
	return false;
}

bool cCreatureVector::Erase(cCreatureBasePtr creature) {
	int foundidx = Find(creature);
	return Remove(foundidx);
}

bool cCreatureVector::Condense() {
	vector<cCreatureBasePtr> newvec = {};
	for (size_t i = 0; i < mpCreatureVec.size(); i++) {
		if (mpCreatureVec[i]) {
			newvec.push_back(mpCreatureVec[i]);
		}
	}
	bool changed = mpCreatureVec.size() != newvec.size();
	mpCreatureVec = newvec;
	return changed;
}



void cCreatureVector::SetVector(vector<cCreatureBasePtr> vector) {
	mpCreatureVec = vector;
}

void cCreatureVector::SetVector(vector<cCreatureAnimalPtr> vector) {
	mpCreatureVec.clear();
	for (size_t i = 0; i < vector.size(); i++) {
		mpCreatureVec.push_back(vector[i]);
	}
}


void cCreatureVector::SetVector(vector<cCreatureCitizenPtr> vector) {
	mpCreatureVec.clear();
	for (size_t i = 0; i < vector.size(); i++) {
		mpCreatureVec.push_back(vector[i]);
	}
}


cCreatureBasePtr cCreatureVector::GetCreatureItem(int index) {
	return mpCreatureVec[index];
}
cCreatureAnimalPtr cCreatureVector::GetAnimalItem(int index) {
	return object_cast<cCreatureAnimal>(mpCreatureVec[index]);
}
cCreatureCitizenPtr cCreatureVector::GetCitizenItem(int index) {
	return object_cast<cCreatureCitizen>(mpCreatureVec[index]);
}

void cCreatureVector::AddToOpenSlot(cCreatureBasePtr creature) {
	for (size_t i = 0; i < mpCreatureVec.size(); i++) {
		if (mpCreatureVec[i] == nullptr) {
			mpCreatureVec[i] = (creature);
			return;
		}
	}
	mpCreatureVec.push_back(creature);
}