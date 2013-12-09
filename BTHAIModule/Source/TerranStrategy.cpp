#include "TerranStrategy.h"
#include "BuildplanEntry.h"
#include "AgentManager.h"
#include "ExplorationSquad.h"
#include "BuildingPlacer.h"
#include "ExplorationManager.h"

TerranStrategy::TerranStrategy()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 11));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Bunker, 11));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 13));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 14));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Academy, 18));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 18));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Refinery, 21));
	buildplan.push_back(BuildplanEntry(TechTypes::Stim_Packs, 21));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 23));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 30));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Engineering_Bay, 30));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 30));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 30));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 26));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 32));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 38));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 44));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 50));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 60));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Factory, 60));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Starport, 60));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Science_Facility, 60));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 60));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 60));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 60));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 60));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Engineering_Bay, 65));

	
	scoutSquad = new Squad(1, Squad::OFFENSIVE, "ScoutSquad", 50);
	scoutSquad->addSetup(UnitTypes::Terran_Marine, 40);
	scoutSquad->addSetup(UnitTypes::Terran_Medic, 10);
	scoutSquad->setRequired(false);
	scoutSquad->setBuildup(false);
	squads.push_back(scoutSquad);

	mainSquad = new Squad(2, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->addSetup(UnitTypes::Terran_Marine, 12);
	mainSquad->addSetup(UnitTypes::Terran_Medic, 3);
	mainSquad->setRequired(true);
	mainSquad->setBuildup(false);
	squads.push_back(mainSquad);
	
	noWorkers = 18;
	noWorkersPerRefinery = 2;
}

TerranStrategy::~TerranStrategy()
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		delete squads.at(i);
	}
	instance = NULL;
}

void TerranStrategy::computeActions()
{
	computeActionsBase();

	int cSupply = Broodwar->self()->supplyUsed() / 2;
	int min = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	//Implement stuff here
	if (mainSquad->isAttacking())
	{
		mainSquad->setRequired(false);
		mainSquad->setActivePriority(1000);
		scoutSquad->setRequired(true);
	}
}
