#include "LurkerAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

LurkerAgent::LurkerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "LurkerAgent";
	
	goal = TilePosition(-1, -1);
}

void LurkerAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, true);
}
