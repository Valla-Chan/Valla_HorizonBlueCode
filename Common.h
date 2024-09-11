#pragma once
#include <Spore\BasicIncludes.h>

float GetDistance(Vector3 point1, Vector3 point2) {
	return abs((point1 - point2).Length());
}