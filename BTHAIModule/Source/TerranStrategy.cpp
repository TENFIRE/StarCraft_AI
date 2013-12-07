#include "TerranStrategy.h"
#include "BuildplanEntry.h"
#include "AgentManager.h"
#include "ExplorationSquad.h"
#include "BuildingPlacer.h"
#include "ExplorationManager.h"
#include "ComsatAgent.h"
#include "CommandCenterAgent.h"

vector<Unit*>	TerranStrategy::CommandCenters	=	vector<Unit*>();
vector<Unit*>	TerranStrategy::ComsatStations	=	vector<Unit*>();

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

	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Comsat_Station, 35));


	
	mainSquad = new Squad( 1, Squad::OFFENSIVE, "MarineSquad", 10 );
	mainSquad->addSetup( UnitTypes::Terran_Marine, 10 );
	mainSquad->addSetup( UnitTypes::Terran_Medic, 10 );
	mainSquad->setRequired( true );
	mainSquad->setBuildup( false );
	mainSquad->setGoal( findChokePoint() );
	squads.push_back( mainSquad );

	positionsToScan	=	vector<Position>();
	addPossibleScanLocations();

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

void TerranStrategy::addPossibleScanLocations()
{
	positionsToScan.clear();

	//	Create a list with positions to scan
	Position	baseLocation	=	getStartLocation( Broodwar->self() )->getPosition();
				
	for( set<BaseLocation*>::const_iterator i = getBaseLocations().begin(); i != getBaseLocations().end(); ++i )
	{
		Position	tPosition	=	(*i)->getPosition();
		//	Don't add the position of our own base
		if( tPosition.getDistance( baseLocation ) != 0 )
			positionsToScan.push_back( (*i)->getPosition() );
	}
}

Position TerranStrategy::getRandomScanLocation()
{
	if( positionsToScan.size() == 0 )
		return Position( -1, -1 );

	//	Randomize a location from the list
	int			tempIndex	=	rand() % (positionsToScan.size() - 1);
	Position	basePos		=	positionsToScan[ tempIndex ];

	//	Remove the location from the list
	positionsToScan.erase( positionsToScan.begin() + tempIndex );

	return basePos;
}

void TerranStrategy::computeActions()
{
	computeActionsBase();

	int	cSupply	=	Broodwar->self()->supplyUsed() / 2;
	int	min		=	Broodwar->self()->minerals();
	int	gas		=	Broodwar->self()->gas();
	int	seconds	=	Broodwar->getFrameCount();	//	Only if fastest


	//	10 seconds intervall
	if( seconds % (160 * 10) == 0 )
	{
		for( int i = ComsatStations.size() - 1; i >= 0; --i )
		{
			Unit*	tComsat	=	ComsatStations[i];

			//	If somehow the unit in the list is not a comsat.
			if( tComsat == NULL )
			{
				ComsatStations.erase( ComsatStations.begin() + i );
				continue;
			}
			//	If the unit is dead remove it
			if( !(tComsat->getHitPoints() > 0) )
			{
				ComsatStations.erase( ComsatStations.begin() + i );
				continue;
			}

			//	If the comsat has enough energy. (100)
			//	100 because then it can use 50 for to scout
			//	and still have 50 in case of invis units.
			if( tComsat->getEnergy() >= 50 )
			{
				Position	basePos	=	getRandomScanLocation();
				Broodwar->sendText( "Scanner Sweep incoming on <%i, %i>", basePos.x(), basePos.y() );

				tComsat->useTech( TechTypes::Scanner_Sweep, basePos );
			}
		}

	}
}
