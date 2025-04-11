#pragma once

/// For tool types past those in Simulator::TribeToolType
enum ToolTypes {
	// only used in the palette or generation
	HomePalette = 11,
	DecorPalette = 12,
	EventRare = 13,
	// Allocate 10 slots for tribal homes
	HomeStart = 14,
	HomeEnd = 24,
	//
	Watchtower = 25,
	//
	Scarecrow = 40,
	Well1 = 54,
	Well2 = 55,
	Well3 = 56,
	//
	Decor = 999,
};