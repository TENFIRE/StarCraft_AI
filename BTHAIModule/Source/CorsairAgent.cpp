#include "CorsairAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

CorsairAgent::CorsairAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "CorsairAgent";
	
	goal = TilePosition(-1, -1);
}

void CorsairAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
