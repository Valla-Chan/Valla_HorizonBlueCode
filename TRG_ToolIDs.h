#pragma once

/// For tool types past those in Simulator::TribeToolType
enum ToolTypes {
	HomePalette = 11, // only used in the palette
	// Allocate 10 slots for tribal homes
	HomeStart = 12,
	HomeEnd = 22,
	//
	EventRare = 23,
	Watchtower = 24,
};