#include "UltraliskAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

UltraliskAgent::UltraliskAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "UltraliskAgent";
	
	goal = TilePosition(-1, -1);
}

void UltraliskAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
