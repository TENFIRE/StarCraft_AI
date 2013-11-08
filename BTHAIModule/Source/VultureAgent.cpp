#include "VultureAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

VultureAgent::VultureAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "VultureAgent";
	
	goal = TilePosition(-1, -1);
}

void VultureAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
