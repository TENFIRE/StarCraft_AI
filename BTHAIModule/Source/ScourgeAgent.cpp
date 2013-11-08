#include "ScourgeAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

ScourgeAgent::ScourgeAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ScourgeAgent";
	
	goal = TilePosition(-1, -1);
}

void ScourgeAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
