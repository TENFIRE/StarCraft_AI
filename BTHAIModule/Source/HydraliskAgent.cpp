#include "HydraliskAgent.h"
#include "NavigationAgent.h"
#include "Commander.h"
#include "TargetingAgent.h"

HydraliskAgent::HydraliskAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "HydraliskAgent";
	
	goal = TilePosition(-1, -1);
}

void HydraliskAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
