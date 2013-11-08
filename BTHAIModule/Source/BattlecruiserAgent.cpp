#include "BattlecruiserAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"

BattlecruiserAgent::BattlecruiserAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "BattlecruiserAgent";
		
	goal = TilePosition(-1, -1);
}

void BattlecruiserAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
