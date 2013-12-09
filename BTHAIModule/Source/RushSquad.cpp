#include "RushSquad.h"
#include "AgentManager.h"

RushSquad::RushSquad()
{
	isBunkerMode	=	false;
	bunkeredUnits	=	0;
}

RushSquad::RushSquad(int mId, int mType, string mName, int mPriority)
{
	this->id = mId;
	this->type = mType;
	this->moveType = AIR;
	this->name = mName;
	this->priority = mPriority;
	morphs = UnitTypes::Unknown;
	activePriority = priority;
	active = false;
	required = false;
	goal = TilePosition(-1, -1);
	goalSetFrame = 0;
	arrivedFrame = -1;
	currentState = STATE_DEFEND;
	buildup = false;

	isBunkerMode	=	false;
	bunkeredUnits	=	0;
	bunkerUnit		=	NULL;
}
void RushSquad::setBunkerMode( bool Value )
{
	isBunkerMode	=	Value;
}
void RushSquad::computeActions()
{
	Squad::computeActions();
	int	seconds	=	Broodwar->getFrameCount();	//	Only if fastest

	//	All logic for when the squad aint active yet
	//	and to keep the stress down only run this once
	//	every few seconds
	if( !isActive() && seconds % (160 * 1) == 0 )
	{
		//	If there are any units in our squad and
		//	the number of bunkered units are below 4 
		//	we can try and add more to the bunker
		if( agents.size() > 0 && isBunkerMode && bunkeredUnits < 4 )
		{
			//	Try and get hold of a bunker
			if( bunkerUnit == NULL )
			{
				BaseAgent*	tBunker	=	AgentManager::getInstance()->getClosestAgent( (TilePosition)agents[0]->getUnit()->getPosition(), UnitTypes::Terran_Bunker );
				if( tBunker == NULL )
					return;

				//	Save the unit
				bunkerUnit	=	tBunker->getUnit();
			}

			//	If no bunker is found, break.
			if( bunkerUnit == NULL )
				return;

			//	Loop through the units and order
			//	all the marines to try and enter the
			//	bunker until it is full.
			for( int i = 0; i < agents.size(); ++i )
			{
				//	 Skip every non marine unit
				if( agents[i]->getUnitType() != UnitTypes::Terran_Marine )
					continue;

				//	Skip the units that are already inside a bunker
				if( agents[i]->getUnit()->isLoaded() )
					continue;

				//	Order the unit to enter the bunker
				agents[i]->getUnit()->rightClick( bunkerUnit, false );
				bunkeredUnits	=	bunkerUnit->getLoadedUnits().size();
			}
		}


		//	If the squad aint active we can stop here
		return;
	}

	//	If the squad is active and should move out
	//	we need to see if there are any bunkered units.
	if( isActive() && isBunkerMode )
	{
		if( bunkerUnit != NULL )
			bunkerUnit->unloadAll( false );

		isBunkerMode	=	false;
	}
}