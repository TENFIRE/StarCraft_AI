#include "InfestedTerranAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

InfestedTerranAgent::InfestedTerranAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "InfestedTerranAgent";
	
	goal = TilePosition(-1, -1);
}

void InfestedTerranAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
