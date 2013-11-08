#include "ScoutAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

ScoutAgent::ScoutAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ScoutAgent";
	
	goal = TilePosition(-1, -1);
}

void ScoutAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
