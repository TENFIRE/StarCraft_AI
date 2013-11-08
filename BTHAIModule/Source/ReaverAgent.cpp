#include "ReaverAgent.h"
#include "NavigationAgent.h"

ReaverAgent::ReaverAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ReaverAgent";
	
	goal = TilePosition(-1, -1);
}

void ReaverAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
