#include "AgentManager.h"
#include "AgentFactory.h"
#include "BuildingPlacer.h"
#include "Commander.h"
#include "Constructor.h"
#include "ResourceManager.h"
#include "WorkerAgent.h"
#include "Profiler.h"

#define AI 1

int AgentManager::StartFrame = 0;
AgentManager* AgentManager::instance = NULL;

AgentManager::AgentManager()
{
	lastCallFrame = Broodwar->getFrameCount();
}

AgentManager::~AgentManager()
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		delete agents.at(i);
	}
	
	instance = NULL;
}

AgentManager* AgentManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new AgentManager();
	}
	return instance;
}

const vector<BaseAgent*>& AgentManager::getAgents()
{
	return agents;
}

int AgentManager::size()
{
	return agents.size();
}

BaseAgent* AgentManager::getAgent(int unitID)
{
	BaseAgent* agent = NULL;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnitID() == unitID)
		{
			agent = agents.at(i);
			break;
		}
	}

	return agent;
}

void AgentManager::requestOverlord(TilePosition pos)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isOfType(UnitTypes::Zerg_Overlord) && agents.at(i)->isAlive())
		{
			if (agents.at(i)->getGoal().x() == -1)
			{
				agents.at(i)->setGoal(pos);
				return;
			}
		}
	}
}

BaseAgent* AgentManager::getAgent(UnitType type)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isOfType(type) && agents.at(i)->isAlive())
		{
			return agents.at(i);
		}
	}
	return NULL;
}

BaseAgent* AgentManager::getClosestBase(TilePosition pos)
{
	BaseAgent* agent = NULL;
	double bestDist = 100000;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnitType().isResourceDepot() && agents.at(i)->isAlive())
		{
			double dist = agents.at(i)->getUnit()->getDistance(Position(pos));
			if (dist < bestDist)
			{
				bestDist = dist;
				agent = agents.at(i);
			}
		}
	}
	return agent;
}

BaseAgent* AgentManager::getClosestAgent(TilePosition pos, UnitType type)
{
	BaseAgent* agent = NULL;
	double bestDist = 100000;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isOfType(type) && agents.at(i)->isAlive())
		{
			double dist = agents.at(i)->getUnit()->getDistance(Position(pos));
			if (dist < bestDist)
			{
				bestDist = dist;
				agent = agents.at(i);
			}
		}
	}
	return agent;
}

void AgentManager::addAgent(Unit* unit)
{
	if (unit->getType().getID() == UnitTypes::Zerg_Larva.getID())
	{
		//Special case: Dont add Zerg larva as agents.
		return;
	}
	if (unit->getType().getID() == UnitTypes::Zerg_Egg.getID())
	{
		//Special case: Dont add Zerg eggs as agents.
		return;
	}
	if (unit->getType().getID() == UnitTypes::Zerg_Cocoon.getID())
	{
		//Special case: Dont add Zerg cocoons as agents.
		return;
	}
	if (unit->getType().getID() == UnitTypes::Zerg_Lurker_Egg.getID())
	{
		//Special case: Dont add Zerg eggs as agents.
		return;
	}

	bool found = false;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->matches(unit))
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		BaseAgent* newAgent = AgentFactory::getInstance()->createAgent(unit);
		agents.push_back(newAgent);

		if (newAgent->isBuilding())
		{
			BuildingPlacer::getInstance()->addConstructedBuilding(unit);
			Constructor::getInstance()->unlock(unit->getType());
			ResourceManager::getInstance()->unlockResources(unit->getType());
		}
		else
		{
			Commander::getInstance()->unitCreated(newAgent);
		}
	}
}

void AgentManager::removeAgent(Unit* unit)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->matches(unit))
		{
			
			if (agents.at(i)->isBuilding())
			{
				BuildingPlacer::getInstance()->buildingDestroyed(unit);
			}

			agents.at(i)->destroyed();
			Commander::getInstance()->unitDestroyed(agents.at(i));

			//Special case: If a bunker is destroyed, we need to remove
			//the bunker squad.
			if (unit->getType().getID() == UnitTypes::Terran_Bunker.getID())
			{
				int squadID = agents.at(i)->getSquadID();
				Commander::getInstance()->removeSquad(squadID);
			}

			return;
		}
	}
}

void AgentManager::morphDrone(Unit* unit)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->matches(unit))
		{
			agents.erase(agents.begin() + i);
			addAgent(unit);
			return;
		}
	}
	//No match found. Add it anyway.
	if (unit->exists())
	{
		addAgent(unit);
	}
}

void AgentManager::cleanup()
{
	//Step 1. Check if any agent is under attack. If so, dont cleanup since
	//it might cause a Nullpointer.
	//Seems to work now
	/*for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isUnderAttack())
		{
			return;
		}
	}*/

	//Step 2. Do the cleanup.
	int cnt = 0;
	int oldSize = (int)agents.size();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (!agents.at(i)->isAlive())
		{
			delete agents.at(i);
			agents.erase(agents.begin() + i);
			cnt++;
			i--;
		}
	}
	int newSize = (int)agents.size();
}

void AgentManager::computeActions()
{
	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 10)
	{
		//return;
	}
	lastCallFrame = cFrame;

	int st = (int)GetTickCount();
	int et = 0;
	int elapsed = 0;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		et = (int)GetTickCount();
		elapsed = et - st;
		if (elapsed >= 30)
		{
			return;
		}

		if (agents.at(i)->isAlive())
		{
			int lastAF = agents.at(i)->getLastActionFrame();
			if (Broodwar->getFrameCount() - lastAF > 20)
			{
				agents.at(i)->setActionFrame();
#if AI == 0
				if (agents.at(i)->getUnitType().isBuilding() || agents.at(i)->getUnitType().isAddon() || agents.at(i)->getUnitType().isWorker())
				{
					agents.at(i)->computeActions();
				}
#else
				agents.at(i)->computeActions();
#endif
			}
		}
	}
}

int AgentManager::getNoWorkers()
{
	int wCnt = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent != NULL && agent->isWorker() && agent->isAlive())
		{
			wCnt++;
		}
	}
	return wCnt;
}

int AgentManager::noMiningWorkers()
{
	int cnt = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isWorker() && agent->isAlive())
		{
			WorkerAgent* w = (WorkerAgent*)agent;
			if (w->getState() == WorkerAgent::GATHER_MINERALS)
			{
				cnt++;
			}
		}
	}
	return cnt;
}

BaseAgent* AgentManager::findClosestFreeWorker(TilePosition pos)
{
	BaseAgent* bestAgent = NULL;
	double bestDist = 10000;

	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isFreeWorker())
		{
			double cDist = agents.at(i)->getUnit()->getDistance(Position(pos));
			if (cDist < bestDist)
			{
				bestDist = cDist;
				bestAgent = agents.at(i);
			}
		}
	}
	return bestAgent;
}

bool AgentManager::isAnyAgentRepairingThisAgent(BaseAgent* repairedAgent)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->isWorker())
		{
			Unit* unit = agent->getUnit();
			if (unit->getTarget() != NULL && unit->getTarget()->getID() == repairedAgent->getUnitID())
			{
				//Already have an assigned builder
				return true;
			}
		}
	}
	return false;
}

int AgentManager::noInProduction(UnitType type)
{
	int cnt = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->isOfType(type) && agents.at(i)->getUnit()->isBeingConstructed())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

bool AgentManager::hasBuilding(UnitType type)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isOfType(type) && agents.at(i)->isAlive())
		{
			if (!agents.at(i)->getUnit()->isBeingConstructed())
			{
				return true;
			}
		}
	}
	return false;
}

int AgentManager::countNoUnits(UnitType type)
{
	int cnt = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->isOfType(type) && agents.at(i)->isAlive())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

int AgentManager::countNoFinishedUnits(UnitType type)
{
	int cnt = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->isOfType(type) && agents.at(i)->isAlive() && !agents.at(i)->getUnit()->isBeingConstructed())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

int AgentManager::countNoBases()
{
	int cnt = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->getUnitType().isResourceDepot() && !agents.at(i)->getUnit()->isBeingConstructed())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

bool AgentManager::unitsInArea(TilePosition pos, int tileWidth, int tileHeight, int unitID)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->getUnit()->getID() != unitID)
			{
				TilePosition aPos = agents.at(i)->getUnit()->getTilePosition();
				if (aPos.x() >= pos.x() && aPos.x() <= pos.x() + tileWidth && aPos.y() >= pos.y() && aPos.y() <= pos.y() + tileWidth)
				{
					return true;
				}
			}
		}
	}
	return false;
}

TilePosition AgentManager::getClosestDetector(TilePosition startPos)
{
	TilePosition pos = TilePosition(-1, -1);
	double bestDist = 10000;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive())
		{
			if (agent->getUnitType().isDetector() && agent->getUnitType().isBuilding())
			{
				double cDist = startPos.getDistance(agent->getUnit()->getTilePosition());
				if (cDist < bestDist)
				{
					bestDist = cDist;
					pos = agent->getUnit()->getTilePosition();
				}
			}
		}
	}

	return pos;
}
