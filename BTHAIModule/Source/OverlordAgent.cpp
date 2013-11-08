#include "OverlordAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"

OverlordAgent::OverlordAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "OverlordAgent";
	
	goal = TilePosition(-1, -1);
}

void OverlordAgent::computeActions()
{
	if (squadID == -1)
	{
		BaseAgent* agent = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
		if (agent != NULL)
		{
			goal = agent->getUnit()->getTilePosition();
			return;
		}
	}

	NavigationAgent::getInstance()->computeMove(this, goal, true);
}
