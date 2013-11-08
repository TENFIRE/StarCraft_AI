#include "ProtossStrategy.h"
#include "BuildplanEntry.h"
#include "ExplorationSquad.h"
#include "ExplorationManager.h"
#include "AgentManager.h"
#include "BuildingPlacer.h"

ProtossStrategy::ProtossStrategy()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Pylon, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Gateway, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Gateway, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Pylon, 14));
	
	mainSquad = new Squad(1, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->addSetup(UnitTypes::Protoss_Zealot, 4);
	mainSquad->setBuildup(false);
	mainSquad->setRequired(true);
	squads.push_back(mainSquad);

	noWorkers = 16;
	noWorkersPerRefinery = 3;
}

ProtossStrategy::~ProtossStrategy()
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		delete squads.at(i);
	}
	instance = NULL;
}

void ProtossStrategy::computeActions()
{
	computeActionsBase();

	int cSupply = Broodwar->self()->supplyUsed() / 2;
	int min = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	//Implement stuff here
}
