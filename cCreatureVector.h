#pragma once

#include <Spore\BasicIncludes.h>

#define cCreatureVectorPtr intrusive_ptr<cCreatureVector>

using namespace Simulator;

class cCreatureVector 
	: public Object
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("cCreatureVector");
	
	vector<cCreatureBasePtr> mpCreatureVec = {};

	cCreatureVector();
	cCreatureVector(vector<cCreatureBasePtr> vector);
	cCreatureVector(vector<cCreatureAnimalPtr> vector);
	cCreatureVector(vector<cCreatureCitizenPtr> vector);
	~cCreatureVector();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;

	vector<cCreatureBasePtr> GetVector();
	void Clear();
	int Find(cCreatureBasePtr creature) const;
	bool Has(cCreatureBasePtr creature) const;
	// Next 2: return true if existed and removed.
	bool Remove(int index); // -1 will refer to the last item
	bool Erase(cCreatureBasePtr creature);
	// Recreate the vector with all nullptrs removed and the size decreased.
	bool Condense();

	void SetVector(vector<cCreatureBasePtr> vector);
	void SetVector(vector<cCreatureAnimalPtr> vector);
	void SetVector(vector<cCreatureCitizenPtr> vector);

	cCreatureBasePtr GetCreatureItem(int index);
	cCreatureAnimalPtr GetAnimalItem(int index);
	cCreatureCitizenPtr GetCitizenItem(int index);

	void AddToOpenSlot(cCreatureBasePtr creature);
};
