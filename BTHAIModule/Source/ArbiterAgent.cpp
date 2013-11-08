#include "ArbiterAgent.h"
#include "NavigationAgent.h"

ArbiterAgent::ArbiterAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ArbiterAgent";
	
	goal = TilePosition(-1, -1);
}

void ArbiterAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, true);
}
