#include "TerranStrategy.h"
#include "BuildplanEntry.h"
#include "AgentManager.h"
#include "ExplorationSquad.h"
#include "BuildingPlacer.h"
#include "ExplorationManager.h"

TerranStrategy::TerranStrategy()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 12));
	
	mainSquad = new Squad(1, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->addSetup(UnitTypes::Terran_Marine, 5);
	mainSquad->setRequired(true);
	mainSquad->setBuildup(false);
	squads.push_back(mainSquad);

	noWorkers = 16;
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
}
