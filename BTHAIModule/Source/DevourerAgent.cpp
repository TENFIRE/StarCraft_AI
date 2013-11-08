#include "DevourerAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

DevourerAgent::DevourerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DevourerAgent";
	
	goal = TilePosition(-1, -1);
}

void DevourerAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
