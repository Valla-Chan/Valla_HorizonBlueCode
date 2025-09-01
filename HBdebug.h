#pragma once

#include <Spore\BasicIncludes.h>

using namespace Editors;
using namespace Simulator;
using namespace UI;
class HBdebug 
	: public ArgScript::ICommand
{
public:
	HBdebug();
	~HBdebug();

	cCreatureAnimalPtr avatar ;
	cPlayer* player;
	cHerdPtr herd;
	cNestPtr nest;
	cTribe* tribe;
	cCreatureCitizenPtr chieftain;

	cGameDataPtr hovered;
	cCreatureAnimalPtr hoveredAnimal;
	cCreatureCitizenPtr hoveredCitizen;
	cCombatantPtr hoveredCombatant;
	// ui
	IWindowPtr window;
	// editor
	cEditor* editor;
	EditorModel* editorModel;
	// CRG part unlocks
	cCollectableItemsPtr crgparts;
	// planet
	cPlanetRecord* planetRecord;
	cEmpirePtr empire;
	cGameDataUFOPtr ship;
	cCivilizationPtr civilization;
	tGameDataVectorT<cCity> cities;
	cCityPtr firstCity;

	void HBdebug::StoreData();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
	
private:
	void CiviliansAttackHovered();
	void TribesAttackShip();
};

