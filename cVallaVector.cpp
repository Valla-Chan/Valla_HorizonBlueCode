#include "stdafx.h"
#include "cVallaVector.h"

void VallaVector::append(vector<cGameDataPtr>vec, const cGameDataPtr value) {
	for (size_t i = 0; i < vec.size(); i++) {
		// TODO: is this gonna cause issues?
		if (vec[i] == nullptr) {
			vec[i] = value;
			return;
		}
	}
	vec.push_back(value);
}

bool VallaVector::append_unique(vector<cGameDataPtr>vec, const cGameDataPtr value) {
	size_t open_index = vec.size(); // if doesnt change from this, then push_back at the end
	for (size_t i = 0; i < vec.size(); i++) {
		if (i < open_index && vec[i] == nullptr) {
			open_index = i;
		}
		// value exists, return false
		else if (vec[i] == value) {
			return false;
		}
	}
	if (open_index < vec.size()) {
		vec[open_index] = value;
	}
	else {
		vec.push_back(value);
	}
	return true;
}

int VallaVector::find(vector<cGameDataPtr>vec, const cGameDataPtr value) {
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i] == value) {
			return i;
		}
	}
	return -1;
}