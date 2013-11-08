#include "ScienceVesselAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

ScienceVesselAgent::ScienceVesselAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ScienceVesselAgent";
	
	goal = TilePosition(-1, -1);
}

void ScienceVesselAgent::computeActions()
{
	NavigationAgent::getInstance()->computeMove(this, goal, true);
}

