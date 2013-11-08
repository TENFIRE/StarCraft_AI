#include "ZealotAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

ZealotAgent::ZealotAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ZealotAgent";
	//Broodwar->printf("ZealotAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ZealotAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
