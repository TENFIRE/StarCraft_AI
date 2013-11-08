#include "GuardianAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

GuardianAgent::GuardianAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GuardianAgent";
	
	goal = TilePosition(-1, -1);
}

void GuardianAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
