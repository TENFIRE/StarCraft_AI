#include "MutaliskAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"

MutaliskAgent::MutaliskAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MutaliskAgent";
	
	goal = TilePosition(-1, -1);
}

void MutaliskAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
