#include "MarineAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"

MarineAgent::MarineAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MarineAgent";
	
	goal = TilePosition(-1, -1);
}

void MarineAgent::computeActions()
{
	//	Stim pack logic
	if( isAttacking() && unit->getStimTimer() <= 0 )
		if( unit->getHitPoints() > 20 )
			unit->useTech( TechTypes::Stim_Packs );

	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
