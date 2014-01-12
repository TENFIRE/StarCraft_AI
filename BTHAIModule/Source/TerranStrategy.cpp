#include "TerranStrategy.h"
#include "BuildplanEntry.h"
#include "AgentManager.h"
#include "ExplorationSquad.h"
#include "BuildingPlacer.h"
#include "ExplorationManager.h"
#include "RushSquad.h"
#include "DistractionSquad.h"
#include "ComsatAgent.h"
#include "CommandCenterAgent.h"

vector<Unit*>	TerranStrategy::CommandCenters	=	vector<Unit*>();
vector<Unit*>	TerranStrategy::ComsatStations	=	vector<Unit*>();

RushSquad*	tSquad;
Squad*		mSquad;
bool		mDeploy;
int			mCounter;
TerranStrategy::TerranStrategy()
{
//Broodwar->enableFlag( Flag::CompleteMapInformation );
/*
	// Start
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 10));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Bunker, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 13));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 14));

	// Supply Depots
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 14));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 16));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 22));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 30));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 36));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 42));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 48));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 56));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 60));

	// Whenever possible after start (in order)
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Refinery, 15));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 15));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Academy, 15));
	buildplan.push_back(BuildplanEntry(TechTypes::Stim_Packs, 15));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Engineering_Bay, 15));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 15));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 15));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Comsat_Station, 15));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Factory, 20));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Machine_Shop, 20));
	buildplan.push_back(BuildplanEntry(TechTypes::Tank_Siege_Mode, 22));

	/*

	/*
	mainSquad = new DistractionSquad(1, Squad::OFFENSIVE, "DistractionSquad", 5);
	mainSquad->addSetup(UnitTypes::Terran_Marine, 2);
	mainSquad->setRequired(true);
	mainSquad->setBuildup(false);
	mainSquad->setGoal(findChokePoint());
	squads.push_back(mainSquad);
	*/

	// Supply Depots
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 14));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 16));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 22));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 30));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 36));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 42));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 48));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 56));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 60));

	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Refinery, 14));

	//	Barracks
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 10));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Bunker, 10));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 18));


	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Academy, 14));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Engineering_Bay, 18));
	buildplan.push_back(BuildplanEntry(TechTypes::Stim_Packs, 20));

	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 22));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 24));

	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Comsat_Station, 28));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Factory, 28));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Machine_Shop, 28));
	buildplan.push_back(BuildplanEntry(TechTypes::Tank_Siege_Mode, 28));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Starport, 35));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Control_Tower, 35));

	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 45));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 45));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, 60));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Armor, 60));
	
	tSquad = new RushSquad( 2, Squad::OFFENSIVE, "MarineSquad", 10 );
	tSquad->addSetup( UnitTypes::Terran_Marine, 12 );
	tSquad->addSetup(UnitTypes::Terran_Medic, 4);
	tSquad->setRequired( true );
	tSquad->setBuildup( false );
	tSquad->setGoal( findChokePoint() );
	tSquad->setBunkerMode( true );
	squads.push_back( tSquad );
	mDeploy		=	false;
	mCounter	=	0;
	
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

	//	Debug message
	//Broodwar->sendText( "Intresting location added <%i, %i> with a factor %i.", P.x(), P.y(), Factor );
}

Position TerranStrategy::getEnemyLocationFromScans()
{
	Position	finalPosition	=	Position(-1, -1);
	int			positionValue	=	99999;
	for( int i = 0; i < enemyLocationsFromScans.size(); ++i )
	{
		LocationData	tData	=	enemyLocationsFromScans[i];
		if( tData.Attractiveness != 0 && tData.Attractiveness < positionValue )
		{
			finalPosition	=	tData.Location;
			positionValue	=	tData.Attractiveness;
		}
	}

	return	finalPosition;
}

void TerranStrategy::deployNewSquad()
{
	//	If there is a current mSquad
	//	force it to attack and null it
	if( mSquad != NULL )
	{
		mSquad->forceActive();
		mSquad->attack( (TilePosition)getEnemyLocationFromScans() );
		mSquad->setPriority( 1000 );
		mSquad->setActivePriority( 1000 );
		mSquad	=	NULL;
	}

	//	The squad
	mSquad	=	new Squad( 500 + mCounter, Squad::OFFENSIVE, "MGSquad", 10 );
	mSquad->addSetup( UnitTypes::Terran_Marine, 15 );
	mSquad->addSetup( UnitTypes::Terran_Medic, 4 );
	mSquad->addSetup( UnitTypes::Terran_Siege_Tank_Tank_Mode, 3 );
	mSquad->setRequired( true );
	mSquad->setBuildup( false );
	mSquad->setGoal( findChokePoint() );
	squads.push_back( mSquad );
	mCounter++;
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

				//	Debug message
				//Broodwar->sendText( "Scanner Sweep incoming on <%i, %i>", basePos.x(), basePos.y() );

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

	if( seconds % ( 160 * 1 ) == 0 )
	{
		if( mSquad != NULL )
		{
			if( mSquad->isFull() )
				deployNewSquad();
		}
	}

	//	Every ~33 seconds try and create a new squad
	if( seconds % ( 160 * 33 ) == 0 && mDeploy)
	{
		//	Force all existing squads to attack.
		for(int i = squads.size() - 1; i >= 0; --i )
		{
			Squad*	tempSquad	=	squads[i];
			if( tempSquad->getSize() > 4 )
			{
				tempSquad->forceActive();
				tempSquad->attack( (TilePosition)getEnemyLocationFromScans() );
				tempSquad->setPriority( 1000 );
				tempSquad->setActivePriority( 1000 );

			}
			else
				tempSquad->disband();
		}

		//	The squad
		deployNewSquad();
	}


	if( !mDeploy )
		if( seconds % (160 * 1) == 0 )
			if( tSquad != NULL )
			{
				if( !tSquad->isActive() )
					return;
					
				tSquad->setPriority( 1000 );
				tSquad->setActivePriority( 1000 );
				mDeploy	=	true;

				//	The squad
				mSquad	=	new Squad( 500 + mCounter, Squad::OFFENSIVE, "MGSquad", 10 );
				mSquad->addSetup( UnitTypes::Terran_Marine, 15 );
				mSquad->addSetup( UnitTypes::Terran_Medic, 4 );
				mSquad->addSetup( UnitTypes::Terran_Siege_Tank_Tank_Mode, 3 );
				mSquad->setRequired( true );
				mSquad->setBuildup( false );
				mSquad->setGoal( findChokePoint() );
				squads.push_back( mSquad );
				mCounter++;
			}
			else
				mDeploy	=	true;
}