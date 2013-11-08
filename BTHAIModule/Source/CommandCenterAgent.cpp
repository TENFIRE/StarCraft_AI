#include "CommandCenterAgent.h"
#include "Commander.h"
#include "AgentManager.h"
#include "Constructor.h"

CommandCenterAgent::CommandCenterAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("New base created at (%d,%d)", unit->getTilePosition().x(), unit->getTilePosition().y());

	hasSentWorkers = false;
	if (AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Command_Center) == 0)
	{
		//We dont do this for the first Command Center.
		hasSentWorkers = true;
	}

	agentType = "CommandCenterAgent";
	Constructor::getInstance()->commandCenterBuilt();
}

void CommandCenterAgent::computeActions()
{
	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;

			Constructor::getInstance()->addRefinery();
		}
	}

	if (!unit->isIdle())
	{
		//Already doing something
		return;
	}

	//Build comsat addon
	if (unit->getAddon() == NULL)
	{
		if (Broodwar->canMake(unit, UnitTypes::Terran_Comsat_Station))
		{
			unit->buildAddon(UnitTypes::Terran_Comsat_Station);
			return;
		}
	}

	if (AgentManager::getInstance()->countNoUnits(Broodwar->self()->getRace().getWorker()) < Commander::getInstance()->getNoWorkers())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}
}
