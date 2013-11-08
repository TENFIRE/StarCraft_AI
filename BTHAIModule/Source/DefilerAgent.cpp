#include "DefilerAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

DefilerAgent::DefilerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DefilerAgent";
	
	goal = TilePosition(-1, -1);
}

void DefilerAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
