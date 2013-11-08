#include "DarkTemplarAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

DarkTemplarAgent::DarkTemplarAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DarkTemplarAgent";
	
	goal = TilePosition(-1, -1);
}

void DarkTemplarAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
