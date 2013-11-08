#include "ObserverAgent.h"
#include "NavigationAgent.h"

ObserverAgent::ObserverAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ObserverAgent";
	
	goal = TilePosition(-1, -1);
}

void ObserverAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, true);
}
