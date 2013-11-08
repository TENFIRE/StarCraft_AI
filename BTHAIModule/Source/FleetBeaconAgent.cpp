#include "FleetBeaconAgent.h"
#include "AgentManager.h"

FleetBeaconAgent::FleetBeaconAgent(Unit* mUnit)
{
	unit = mUnit;
	unitID = unit->getID();
	
	researchOrder.push_back(TechTypes::Disruption_Web);
}

void FleetBeaconAgent::computeActions()
{
	doResearch();
}

string FleetBeaconAgent::getTypeName()
{
	return "FleetBeaconAgent";
}



