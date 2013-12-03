#include "MarineSquad.h"

MarineSquad::MarineSquad()
{
	Squad::Squad();
}

MarineSquad::MarineSquad(int mId, int mType, string mName, int mPriority)
{
	this->id = mId;
	this->type_default = mType;
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
}

void MarineSquad::computeActions()
{
	Squad::computeActions();

	if( Squad::isActive() )
		this->priority	=	1000;

	if( Squad::isUnderAttack() )
	{
		// If the squad is under attack check if
		// we can use Stim Packs
		for( int i = 0; i < agents.size(); ++i )
		{
			Unit*	tUnit	=	agents[i]->getUnit();

			if( tUnit != NULL )
				if( tUnit->getType() == UnitTypes::Terran_Marine && tUnit->isAttacking() )
					if( tUnit->getHitPoints() > 20 && tUnit->getStimTimer() <= 0 )
						tUnit->useTech( TechTypes::Stim_Packs );
		}
	}
}