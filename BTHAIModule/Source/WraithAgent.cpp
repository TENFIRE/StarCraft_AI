#include "WraithAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

WraithAgent::WraithAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "WraithAgent";
	
	goal = TilePosition(-1, -1);
}

void WraithAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
