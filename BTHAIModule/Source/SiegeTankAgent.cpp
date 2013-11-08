#include "SiegeTankAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"
#include "Commander.h"

SiegeTankAgent::SiegeTankAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "SiegeTankAgent";
	
	goal = TilePosition(-1, -1);
}

void SiegeTankAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
