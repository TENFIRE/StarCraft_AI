#include "ZergStrategy.h"
#include "BuildplanEntry.h"
#include "AgentManager.h"
#include "ExplorationSquad.h"
#include "ExplorationManager.h"

ZergStrategy::ZergStrategy()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Spawning_Pool, 5));
	
	mainSquad = new Squad(1, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->addSetup(UnitTypes::Zerg_Zergling, 12);
	mainSquad->setRequired(true);
	mainSquad->setBuildup(false);
	squads.push_back(mainSquad);
	
	noWorkers = 8;
	noWorkersPerRefinery = 3;
}

ZergStrategy::~ZergStrategy()
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		delete squads.at(i);
	}
	instance = NULL;
}

void ZergStrategy::computeActions()
{
	computeActionsBase();

	int cSupply = Broodwar->self()->supplyUsed() / 2;
	int min = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	//Implement stuff here
}
