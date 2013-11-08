#include "ZerglingAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

ZerglingAgent::ZerglingAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ZerglingAgent";
	//Broodwar->printf("ZerglingAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ZerglingAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
