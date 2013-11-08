#include "CarrierAgent.h"
#include "NavigationAgent.h"

CarrierAgent::CarrierAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "CarrierAgent";
	
	goal = TilePosition(-1, -1);
}

void CarrierAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, true);
}
