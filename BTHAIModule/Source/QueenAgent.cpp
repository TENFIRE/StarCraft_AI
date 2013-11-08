#include "QueenAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"

QueenAgent::QueenAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "QueenAgent";
	
	goal = TilePosition(-1, -1);
}

void QueenAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
