#pragma once
#include <Spore\BasicIncludes.h>

class VallaVector
	//: public IWinProc
	//, public DefaultRefCounted
{
public:
	///  Append the value to the nearest open slot, or if none are open, to the end. probably only works with pointer types.
	static void append(vector<cGameDataPtr>vec, const cGameDataPtr value);
	///  Append only if the value doesnt already exist. return true if succeeded in appending value, return false if it already exists.
	static bool append_unique(vector<cGameDataPtr>vec, const cGameDataPtr value);
	/// Find and return the first index of a value
	static int find(vector<cGameDataPtr>vec, const cGameDataPtr value);
};