#include "TerranStrategy.h"
#include "BuildplanEntry.h"
#include "AgentManager.h"
#include "ExplorationSquad.h"
#include "BuildingPlacer.h"
#include "ExplorationManager.h"
#include "MarineSquad.h"


Squad*	rushSquad;
TerranStrategy::TerranStrategy()
{
	// Supply Depots
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 11));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 13));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 20));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 28));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 34));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 40));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 46));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 53));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 60));

	//	Barracks
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 10));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 14));


	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Engineering_Bay, 18));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Refinery, 20));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 22));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 24));

	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Academy, 28));
	buildplan.push_back(BuildplanEntry(TechTypes::Stim_Packs, 30));


	
	rushSquad = new MarineSquad( 1, Squad::OFFENSIVE, "MarineSquad", 10 );
	rushSquad->addSetup( UnitTypes::Terran_Marine, 10 );
	rushSquad->addSetup( UnitTypes::Terran_Medic, 10 );
	rushSquad->setRequired( true );
	rushSquad->setBuildup( false );
	rushSquad->setGoal( findChokePoint() );
	squads.push_back( rushSquad );




	noWorkers = 18;
	noWorkersPerRefinery = 2;
}

TerranStrategy::~TerranStrategy()
{
	for ( int i = 0; i < (int)squads.size(); i++ )
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
