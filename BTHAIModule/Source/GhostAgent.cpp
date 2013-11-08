#include "GhostAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

GhostAgent::GhostAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GhostAgent";
	
	goal = TilePosition(-1, -1);
}

void GhostAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
