#include "ValkyrieAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

ValkyrieAgent::ValkyrieAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ValkyrieAgent";
	
	goal = TilePosition(-1, -1);
}

void ValkyrieAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
