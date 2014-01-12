#include "SiegeTankAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"
#include "Commander.h"

SiegeTankAgent::SiegeTankAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "SiegeTankAgent";
	
	useSiegeMode = true;

	goal = TilePosition(-1, -1);
}

void SiegeTankAgent::computeActions()
{

	if (!unit->isBeingConstructed() && useSiegeMode)
	{
		Squad* squad = Commander::getInstance()->getSquad(this->squadID);

		bool isSieged = unit->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode;
		bool atGoal = getGoal().getDistance(unit->getTilePosition()) < 2;

		bool forceSiege = this->getClosestOrganicEnemy(1) != NULL;
		bool toggleSiege = (atGoal != isSieged) || (forceSiege && !isSieged);

		Broodwar->printf(" ");
		Broodwar->printf(" ");
		Broodwar->printf(" ");
		if (forceSiege)
			Broodwar->printf("target close");
		if (isSieged)
			Broodwar->printf("isSieged");
		if (atGoal)
			Broodwar->printf("atGoal");

		if (toggleSiege)
		{
			unit->useTech(TechTypes::Tank_Siege_Mode);
		}

	}

	NavigationAgent::getInstance()->computeMove(this, goal, false);
}
