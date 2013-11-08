#include "HighTemplarAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "Commander.h"

HighTemplarAgent::HighTemplarAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "HighTemplarAgent";
	
	goal = TilePosition(-1, -1);
}

void HighTemplarAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, true);
}
