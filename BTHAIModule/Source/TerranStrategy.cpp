#include "TerranStrategy.h"
#include "BuildplanEntry.h"
#include "AgentManager.h"
#include "ExplorationSquad.h"
#include "BuildingPlacer.h"
#include "ExplorationManager.h"
#include "RushSquad.h"
#include "ComsatAgent.h"
#include "CommandCenterAgent.h"

vector<Unit*>	TerranStrategy::CommandCenters	=	vector<Unit*>();
vector<Unit*>	TerranStrategy::ComsatStations	=	vector<Unit*>();


TerranStrategy::TerranStrategy()
{
Broodwar->enableFlag( Flag::CompleteMapInformation );
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
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 10));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 14));


	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Engineering_Bay, 18));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Refinery, 20));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 22));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 24));

	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Academy, 25));
	buildplan.push_back(BuildplanEntry(TechTypes::Stim_Packs, 30));

	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Comsat_Station, 28));

	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Bunker, 10));

	
	RushSquad*	tSquad;
	tSquad = new RushSquad( 1, Squad::OFFENSIVE, "MarineSquad", 10 );
	tSquad->addSetup( UnitTypes::Terran_Marine, 10 );
	tSquad->setRequired( true );
	tSquad->setBuildup( false );
	tSquad->setGoal( findChokePoint() );
	tSquad->setBunkerMode( true );
	squads.push_back( tSquad );
	
	enemyLocationsFromScans	=	vector<LocationData>();
	positionsToScan			=	vector<Position>();
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

Position TerranStrategy::getNextScanPosition()
{
	if( positionsToScan.size() == 0 )
		return Position( -1, -1 );

	float		furtherstBase	=	0.0f;
	Position	baseLocation	=	getStartLocation( Broodwar->self() )->getPosition();	
	int			positionIndex	=	0;
	for( int i = 0; i < positionsToScan.size(); ++i )
	{
		float	tDistance	=	baseLocation.getDistance( positionsToScan[i] );
		if( tDistance > furtherstBase )
		{
			positionIndex	=	i;
			furtherstBase	=	tDistance;
		}
	}
	Position	basePos		=	positionsToScan[ positionIndex ];

	//	Remove the location from the list
	positionsToScan.erase( positionsToScan.begin() + positionIndex );

	return basePos;
}

void TerranStrategy::addEnemyLocation( Position P, int Factor )
{
	bool	isAdded			=	false;
	float	similiarRange	=	1000.0f;
	for( int i = 0; i < enemyLocationsFromScans.size(); ++i )
	{
		LocationData	lData	=	enemyLocationsFromScans[i];
		if( P.getDistance( lData.Location ) < similiarRange )
		{
			//	See if the current position should be updated
			if( lData.Attractiveness < Factor )
				enemyLocationsFromScans[i].Attractiveness	=	Factor;

			isAdded	=	true;
			break;
		}
	}

	if( isAdded )
		return;

	LocationData	tData;
	tData.Location			=	P;
	tData.Attractiveness	=	Factor;
	enemyLocationsFromScans.push_back( tData );

	Broodwar->sendText( "Intresting location added <%i, %i> with a factor %i.", P.x(), P.y(), Factor );
}

void TerranStrategy::computeActions()
{
	computeActionsBase();

	int	cSupply	=	Broodwar->self()->supplyUsed() / 2;
	int	min		=	Broodwar->self()->minerals();
	int	gas		=	Broodwar->self()->gas();
	int	seconds	=	Broodwar->getFrameCount();	//	Only if fastest

	//	11 seconds intervall
	if( seconds % (160 * 11) == 0 )
	{
		//	Radius for the sweep scan
		float	sweepRadius	=	320;

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
				//	Fetch the next position to scan
				Position	basePos	=	getNextScanPosition();

				Broodwar->sendText( "Scanner Sweep incoming on <%i, %i>", basePos.x(), basePos.y() );

				//	Cast the Sweep on the position
				tComsat->useTech( TechTypes::Scanner_Sweep, basePos );

				//	Loop through all units in the game, not very
				//	performance effiencent but it was the only
				//	way I found that would actually find units...
				//	All others either returned 0, or only neutrals.
				int	numberOfUnits	=	0;
				for(set<Unit*>::const_iterator i=Broodwar->getAllUnits().begin();i!=Broodwar->getAllUnits().end();i++)
				{
					Unit*	tUnit	=	(*i);

					//	Check so the unit first of all
					//	is an enemy and then so it is 
					//	wihtin the scanning range. 
					//	(sweepRadius is just a approximation)
					if( tUnit->getPlayer()->isEnemy( Broodwar->self() ) )
						if( tUnit->getPosition().getDistance( basePos ) < sweepRadius )
							++numberOfUnits;
				}

				//	If units are within area, add it
				if( numberOfUnits != 0 )
					addEnemyLocation( basePos, numberOfUnits );

			}
		}
	}
}
