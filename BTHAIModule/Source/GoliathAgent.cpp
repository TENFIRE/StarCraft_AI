#include "GoliathAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

GoliathAgent::GoliathAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GoliathAgent";
	
	goal = TilePosition(-1, -1);
}

void GoliathAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
