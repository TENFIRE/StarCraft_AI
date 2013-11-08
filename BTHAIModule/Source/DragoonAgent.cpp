#include "DragoonAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

DragoonAgent::DragoonAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DragoonAgent";
	
	goal = TilePosition(-1, -1);
}

void DragoonAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
