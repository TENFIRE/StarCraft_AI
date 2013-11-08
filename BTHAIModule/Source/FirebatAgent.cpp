#include "FirebatAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"

FirebatAgent::FirebatAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "FirebatAgent";
	
	goal = TilePosition(-1, -1);
}

void FirebatAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
