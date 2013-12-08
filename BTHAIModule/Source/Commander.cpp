#include "Commander.h"
#include "StrategySelector.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "Constructor.h"
#include "Upgrader.h"
#include "Pathfinder.h"
#include "WorkerAgent.h"
#include "DefenseLocator.h"
#include "Profiler.h"
#include <algorithm>
#include <sstream>

#define AI 1

Commander* Commander::instance = NULL;

Commander::Commander()
{
	currentState = DEFEND;
	stage = 0;

	ownDeadScore = 0;
	enemyDeadScore = 0;

	debug_bp = false;
	debug_sq = false;

	lastCallFrame = Broodwar->getFrameCount();
}

Commander::~Commander()
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		delete squads.at(i);
	}
	instance = NULL;
}

Commander* Commander::getInstance()
{
	if (instance == NULL)
	{
		instance = StrategySelector::getInstance()->getStrategy();
	}
	return instance;
}

void Commander::checkBuildplan()
{
	int cSupply = Broodwar->self()->supplyUsed() / 2;

	for (int i = 0; i < (int)buildplan.size(); i++)
	{
		if (cSupply >= buildplan.at(i).supply)
		{
			if (buildplan.at(i).type == BuildplanEntry::BUILDING)
			{
				Constructor::getInstance()->addBuilding(buildplan.at(i).unittype);
				buildplan.erase(buildplan.begin() + i);
				i--;
			}
			else if (buildplan.at(i).type == BuildplanEntry::UPGRADE)
			{
				Upgrader::getInstance()->addUpgrade(buildplan.at(i).upgradetype);
				buildplan.erase(buildplan.begin() + i);
				i--;
			}
			else if (buildplan.at(i).type == BuildplanEntry::TECH)
			{
				Upgrader::getInstance()->addTech(buildplan.at(i).techtype);
				buildplan.erase(buildplan.begin() + i);
				i--;
			}
		}
	}
}

void Commander::cutWorkers()
{
	noWorkers = AgentManager::getInstance()->getNoWorkers();
	Broodwar->printf("Worker production halted: %d", noWorkers);
}

int Commander::getNoWorkers()
{
	return noWorkers;
}

int Commander::getWorkersPerRefinery()
{
	return noWorkersPerRefinery;
}

bool Commander::shallEngage()
{
	TilePosition closeEnemy = getClosestEnemyBuilding(Broodwar->self()->getStartLocation());
	if (closeEnemy.x() == -1)
	{
		//No enemy sighted. Dont launch attack.
		return false;
	}

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->isRequired() && !squads.at(i)->isActive())
		{
			return false;
		}
	}
	return true;
}

void Commander::updateGoals()
{
	TilePosition defSpot = findChokePoint();
	
	if (defSpot.x() != -1)
	{
		for (int i = 0; i < (int)squads.size(); i++)
		{
			Squad* sq = squads.at(i);
			squads.at(i)->defend(defSpot);
		}
	}
}

void Commander::debug_showGoal()
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		squads.at(i)->debug_showGoal();
	}
}

void Commander::computeActions()
{
	//Implemented in the subclasses.
}

void Commander::computeActionsBase()
{
	checkBuildplan();

	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 10)
	{
		return;
	}
	lastCallFrame = cFrame;

#if AI == 1
	//Check if we shall launch an attack
	if (currentState == DEFEND)
	{
		if (shallEngage())
		{
			forceAttack();
		}
	}

	//Check if we shall go back to defend
	if (currentState == ATTACK)
	{
		bool activeFound = false;
		for (int i = 0; i < (int)squads.size(); i++)
		{
			if (squads.at(i)->isRequired() && squads.at(i)->isActive())
			{
				activeFound = true;
			}
		}

		//No active required squads found.
		//Go back to defend.
		if (!activeFound)
		{
			currentState = DEFEND;
			TilePosition defSpot = findChokePoint();
			for (int i = 0; i < (int)squads.size(); i++)
			{
				squads.at(i)->setGoal(defSpot);
			}
		}
	}

	if (currentState == DEFEND)
	{
		//Check if we need to attack/kite enemy workers in the base
		checkWorkersAttack(AgentManager::getInstance()->getClosestBase(Broodwar->self()->getStartLocation()));

		TilePosition defSpot = findChokePoint();
		for (int i = 0; i < (int)squads.size(); i++)
		{
			if (!squads.at(i)->hasGoal())
			{
				if (defSpot.x() != -1)
				{
					squads.at(i)->defend(defSpot);
				}
			}
		}
	}

	if (currentState == ATTACK)
	{
		for (int i = 0; i < (int)squads.size(); i++)
		{
			if (squads.at(i)->isOffensive())
			{
				if (!squads.at(i)->hasGoal())
				{
					TilePosition closeEnemy = getClosestEnemyBuilding(TilePosition(squads.at(i)->getCenter()));
					if (closeEnemy.x() >= 0)
					{
						squads.at(i)->attack(closeEnemy);
					}
				}
			}
			else
			{
				TilePosition defSpot = findChokePoint();
				if (defSpot.x() != -1)
				{
					squads.at(i)->defend(defSpot);
				}
			}
		}
	}
	
	//Compute Squad actions.
	int st = (int)GetTickCount();
	int et = 0;
	int elapsed = 0;

	for(int i = 0; i < (int)squads.size(); i++)
	{
		et = (int)GetTickCount();
		elapsed = et - st;
		if (elapsed >= 30)
		{
			return;
		}
		squads.at(i)->computeActions();
	}

	//Check if any own buildings is under attack, if so try to assist them
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (!agent->getUnit()->isLoaded())
		{
			if (agent->isAlive() && agent->isBuilding() && agent->isUnderAttack())
			{
				assistBuilding(agent);
			}
			if (agent->isAlive() && agent->isWorker() && agent->isUnderAttack())
			{
				assistWorker(agent);
			}
		}
	}

	//Attack if we have filled all supply spots
	if (currentState == DEFEND)
	{
		int supplyUsed = Broodwar->self()->supplyUsed() / 2;
		if (supplyUsed >= 198)
		{
			forceAttack();
		}
	}
#endif

	//Check if there are obstacles we can remove. Needed for some
	//strange maps.
	if (Broodwar->getFrameCount() % 150 == 0)
	{
		checkRemovableObstacles();
	}

	//Terran only: Check for repairs and finish unfinished buildings
	if (Constructor::isTerran())
	{
		//Check if there are unfinished buildings we need
		//to complete.
		checkUnfinishedBuildings();
	}

	//Check for units not belonging to a squad
	checkNoSquadUnits();
}

void Commander::checkNoSquadUnits()
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		
		bool notAssigned = true;
		if (!agent->isAlive()) notAssigned = false;
		if (agent->getUnitType().isWorker()) notAssigned = false;
		if (!agent->getUnit()->exists()) notAssigned = false;
		if (agent->isOfType(UnitTypes::Zerg_Overlord)) notAssigned = false;
		if (agent->getUnitType().isBuilding()) notAssigned = false;
		if (agent->getUnitType().isAddon()) notAssigned = false;
		if (agent->getSquadID() != -1) notAssigned = false;

		if (notAssigned)
		{
			assignUnit(agent);
		}
	}
}

void Commander::assignUnit(BaseAgent* agent)
{
	//Broodwar->printf("%s (%s) is not assigned to a squad", agent->getUnitType().getName().c_str(), agent->getTypeName().c_str());

	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->needUnit(agent->getUnitType()))
		{
			sq->addMember(agent);
			//Broodwar->printf("%s is assigned to SQ %d", agent->getUnitType().getName().c_str(), sq->getID());
			return;
		}
	}
}

TilePosition Commander::getClosestEnemyBuilding(TilePosition start)
{
	Unit* closestBuilding = NULL;
	SpottedObject* closestObj = NULL;
	double bestDist = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->getType().isBuilding())
		{
			double cDist = start.getDistance((*i)->getTilePosition());
			
			if (!ExplorationManager::canReach(start, (*i)->getTilePosition()))
			{
				//cDist = -1;
			}

			if (bestDist < 0 || cDist < bestDist)
			{
				bestDist = cDist;
				closestBuilding = (*i);
			}
		}
	}

	if (closestBuilding != NULL)
	{
		//Broodwar->printf("[TSC]: Closest enemy building is %s", closestBuilding->getType().getName().c_str());
		return closestBuilding->getTilePosition();
	}

	if (ExplorationManager::getInstance()->isActive())
	{
		bestDist = -1;
		vector<SpottedObject*> units = ExplorationManager::getInstance()->getSpottedBuildings();
		for (int i = 0; i < (int)units.size(); i++)
		{
			SpottedObject* obj = units.at(i);
			if (obj->getType().isBuilding() && obj->isActive())
			{
				double cDist = obj->getDistance(start);
				if (bestDist < 0 || cDist < bestDist)
				{
					bestDist = cDist;
					closestObj = obj;
				}
			}
		}

		if (closestObj != NULL)
		{
			//Broodwar->printf("[TSC]: Closest enemy building is %s", closestBuilding->getType().getName().c_str());
			return closestObj->getTilePosition();
		}
	}

	//No building has been spotted. Move towards a starting point.
	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->isOffensive() && squads.at(i)->getSize() > 0)
		{
			TilePosition nextArea = squads.at(i)->getNextStartLocation();
			return nextArea;
		}
	}
	
	return TilePosition(-1,-1);
}

void Commander::removeSquad(int id)
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->getID() == id)
		{
			sq->disband();
			squads.erase(squads.begin() + i);
			return;
		}
	}	
}

Squad* Commander::getSquad(int id)
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->getID() == id)
		{
			return squads.at(i);
		}
	}
	return NULL;
}

vector<Squad*> Commander::getSquads()
{
	return squads;
}

void Commander::unitDestroyed(BaseAgent* agent)
{
	int squadID = agent->getSquadID();
	if (squadID != -1)
	{
		Squad* squad = getSquad(squadID);
		if (squad != NULL)
		{
			squad->removeMember(agent);
		}
	}
}

void Commander::sortSquadList()
{
	sort(squads.begin(), squads.end(), SortSquadList());
}

void Commander::unitCreated(BaseAgent* agent)
{
	//Sort the squad list
	sortSquadList();

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->addMember(agent))
		{
			break;
		}
	}
}

bool Commander::checkWorkersAttack(BaseAgent *base)
{
	int noAttack = 0;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists() && (*i)->getType().isWorker())
		{
			double dist = (*i)->getTilePosition().getDistance(base->getUnit()->getTilePosition());
			if (dist <= 12)
			{
				//Enemy unit discovered. Attack with some workers.
				vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
				for (int j = 0; j < (int)agents.size(); j++)
				{
					BaseAgent* agent = agents.at(j);
					if (agent->isAlive() && agent->isWorker() && noAttack < 1)
				{
						WorkerAgent* wAgent = (WorkerAgent*) agent;
						wAgent->setState(WorkerAgent::ATTACKING);
						agent->getUnit()->attack((*i));
						noAttack++;
					}
				}
			}
		}
	}

	if (noAttack > 0)
	{
		return true;
	}
	return false;
}

void Commander::checkRemovableObstacles()
{
	for(set<Unit*>::iterator m = Broodwar->getMinerals().begin(); m != Broodwar->getMinerals().end(); m++)
	{
		if ((*m)->getResources() <= 20)
		{
			//Found a mineral that we can remove.
			BaseAgent* baseAgent = AgentManager::getInstance()->getClosestBase((*m)->getTilePosition());
			if (baseAgent != NULL)
			{
				double cDist = baseAgent->getUnit()->getDistance((*m));
				if (cDist < 1000)
				{
					//It is close to a base, remove it.

					//Step 1: Check if someone already is working on it
					bool assign = true;
					vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
					for (int i = 0; i < (int)agents.size(); i++)
					{
						BaseAgent* agent = agents.at(i);
						if (agent->isWorker())
						{
							Unit* u = agent->getUnit();
							if (u->isGatheringMinerals())
							{
								Unit* t = u->getTarget();
								if (t != NULL && t->getID() == u->getID())
								{
									//Someone already working on it. Dont bother.
									assign = false;
								}
							}
						}
					}

					if (assign)
					{
						BaseAgent* worker = AgentManager::getInstance()->findClosestFreeWorker((*m)->getTilePosition());
						if (worker != NULL)
						{
							worker->getUnit()->rightClick((*m));
						}
					}
				}
			}
		}
	}
}

TilePosition Commander::findUnfortifiedChokePoint()
{
	double bestDist = 0;
	Chokepoint* bestChoke = NULL;

	for(set<BWTA::Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
	{
		if (isOccupied((*i)))
		{
			for(set<Chokepoint*>::const_iterator c=(*i)->getChokepoints().begin();c!=(*i)->getChokepoints().end();c++)
			{
				if (isEdgeChokepoint((*c)))
				{
					if (!chokePointFortified(TilePosition((*c)->getCenter())))
				{
						double cDist = Broodwar->self()->getStartLocation().getDistance(TilePosition((*c)->getCenter()));
						if (cDist > bestDist)
				{
							bestDist = cDist;
							bestChoke = (*c);
						}
					}
				}
			}
		}
	}

	TilePosition buildPos = TilePosition(-1, -1);
	if (bestChoke != NULL)
	{
		buildPos = TilePosition(bestChoke->getCenter());
	}
	return buildPos;
}

bool Commander::chokePointFortified(TilePosition center)
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->isOfType(UnitTypes::Terran_Bunker) || agents.at(i)->isOfType(UnitTypes::Protoss_Photon_Cannon) || agents.at(i)->isOfType(UnitTypes::Zerg_Sunken_Colony) || agents.at(i)->isOfType(UnitTypes::Zerg_Creep_Colony) || agents.at(i)->isOfType(UnitTypes::Zerg_Spore_Colony))
			{
				double dist = center.getDistance(agents.at(i)->getUnit()->getTilePosition());
				if (dist <= 20)
				{
					return true;
				}
			}
		}
	}
	return false;
}

double Commander::getChokepointPrio(TilePosition center)
{
	TilePosition ePos = ExplorationManager::getInstance()->getClosestSpottedBuilding(center);

	if (ePos.x() >= 0)
	{
		double dist = ePos.getDistance(center);
		return 1000 - dist;
	}
	else
	{
		double dist = Broodwar->self()->getStartLocation().getDistance(center);
		return dist;
	}
}

TilePosition Commander::findChokePoint()
{
	//First, check the DefenseLocator
	//for a stored defense position.
	DefenseLocator* df = DefenseLocator::getInstance();
	TilePosition storedPos = df->getBaseDefensePos(Broodwar->mapHash());
	if (storedPos.x() != -1) return storedPos;

	double bestPrio = -1;
	Chokepoint*		bestChoke		=	NULL;
	Chokepoint*		wrongChoke		=	NULL;
	set<BWTA::Region*>::const_iterator	regionToSearch;
	
	for(set<BWTA::Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
	{
		if (isOccupied((*i)))
		{
			for(set<Chokepoint*>::const_iterator c=(*i)->getChokepoints().begin();c!=(*i)->getChokepoints().end();c++)
			{
				if (isEdgeChokepoint((*c)))
				{
					double cPrio = getChokepointPrio(TilePosition((*c)->getCenter()));
					if (cPrio > bestPrio)
					{
						bestPrio		=	cPrio;
						wrongChoke		=	(*c);
						regionToSearch	=	i;
					}
				}
			}
		}
	}

	bestPrio	=	-1;
	for(set<BWTA::Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
	{
		if (isOccupied((*i)))
		{
			for(set<Chokepoint*>::const_iterator c=(*i)->getChokepoints().begin();c!=(*i)->getChokepoints().end();c++)
			{
				if( wrongChoke != (*c) )
					if ( isEdgeChokepoint((*c)) )
					{
						double	cPrio	=	getChokepointPrio( TilePosition((*c)->getCenter()) );
						if (cPrio > bestPrio)
						{
							bestPrio = cPrio;
							bestChoke = (*c);
						}
					}
			}
		}
	}


	TilePosition guardPos = Broodwar->self()->getStartLocation();
	if (bestChoke != NULL)
	{
		guardPos = findDefensePos(bestChoke);
		//guardPos = TilePosition(bestChoke->getCenter());

		//Pre-calculate path
		TilePosition b = ExplorationManager::getInstance()->getClosestSpottedBuilding(guardPos);
		if (b.x() >= 0)
		{
			Pathfinder::getInstance()->requestPath(guardPos, b);
		}
	}

	return guardPos;
}

TilePosition Commander::findDefensePos(Chokepoint* choke)
{
	TilePosition defPos = TilePosition(choke->getCenter());
	TilePosition chokePos = defPos;

	double size = choke->getWidth();
	if (size <= 32 * 3)
	{
		//Very narrow chokepoint, dont crowd it
		double bestDist = 10000;
		TilePosition basePos = Broodwar->self()->getStartLocation();

		int maxD = 3;
		int minD = 2;

		//We found a chokepoint. Now we need to find a good place to defend it.
		for (int cX = chokePos.x() - maxD; cX <= chokePos.x() + maxD; cX++)
		{
			for (int cY = chokePos.y() - maxD; cY <= chokePos.y() + maxD; cY++)
			{
				TilePosition cPos = TilePosition(cX, cY);
				if (ExplorationManager::canReach(basePos, cPos))
				{
					double chokeDist = chokePos.getDistance(cPos);
					double baseDist = basePos.getDistance(cPos);

					if (chokeDist >= minD && chokeDist <= maxD)
					{
						if (baseDist < bestDist)
						{
							bestDist = baseDist;
							defPos = cPos;
						}
					}
				}
			}
		}
	}

	//Make defenders crowd around defensive structures.
	if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
	{
		UnitType defType;
		if (Constructor::isZerg()) defType = UnitTypes::Zerg_Sunken_Colony;
		if (Constructor::isProtoss()) defType = UnitTypes::Protoss_Photon_Cannon;
		if (Constructor::isTerran()) defType = UnitTypes::Terran_Bunker;
		
		BaseAgent* turret = AgentManager::getInstance()->getClosestAgent(defPos, defType);
		if (turret != NULL)
		{
			TilePosition tPos = turret->getUnit()->getTilePosition();
			double dist = tPos.getDistance(defPos);
			if (dist <= 22)
			{
				defPos = tPos;
			}
		}
	}

	return defPos;
}

bool Commander::isEdgeChokepoint(Chokepoint* choke)
{
	pair<BWTA::Region*,BWTA::Region*> regions = choke->getRegions();
	//If both is occupied it is not an edge chokepoint
	if (isOccupied(regions.first) && isOccupied(regions.second))
	{
		return false;
	}
	//...but one of them must be occupied
	if (isOccupied(regions.first) || isOccupied(regions.second))
	{
		return true;
	}
	return false;
}

bool Commander::isOccupied(BWTA::Region* region)
{
	BWTA::Polygon p = region->getPolygon();
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->getUnitType().isResourceDepot())
		{
			BWTA::Region* aRegion = getRegion(agents.at(i)->getUnit()->getTilePosition());
			Position c1 = region->getCenter();
			Position c2 = aRegion->getCenter();
			if (c2.x() == c1.x() && c2.y() == c1.y())
			{
				return true;
			}
		}
	}
	
	//Check expansion site
	TilePosition expansionSite = ExplorationManager::getInstance()->getExpansionSite();
	TilePosition center = TilePosition(region->getCenter());
	if (expansionSite.x() >= 0)
	{
		double dist = expansionSite.getDistance(center);
		if (dist <= 15)
		{
			return true;
		}
	}

	return false;
}

bool Commander::needUnit(UnitType type)
{
	int prevPrio = 1000;

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (!squads.at(i)->isFull())
		{
			if (squads.at(i)->getPriority() > prevPrio)
			{
				return false;
			}

			if (squads.at(i)->needUnit(type))
			{
				return true;
			}
			
			prevPrio = squads.at(i)->getPriority();
		}
	}
	return false;
}

void Commander::assistBuilding(BaseAgent* building)
{
	if (!building->isAlive()) return;
	if (!building->getUnit()->exists()) return;
	
	//Find out who targets the building
	TilePosition defPos = building->getUnit()->getTilePosition();
	Unit* target = NULL;
	int bestScore = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			double dist = (*i)->getDistance(Position(defPos));
			if (dist <= 13 * 32)
			{
				//Found a target
				if ((*i)->getType().destroyScore() > bestScore)
				{
					target = (*i);
					bestScore = (*i)->getType().destroyScore();
				}
			}
		}
	}

	if (target != NULL)
	{
		//Broodwar->printf("Assisting building: Targeting enemy %s", target->getType().getName().c_str());
		defPos = target->getTilePosition();
	}
	
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		bool ok = true;
		if (sq->isExplorer()) ok = false;
		if (sq->isBunkerDefend()) ok = false;
		if (sq->isRush() && sq->isActive()) ok = false;

		if (ok)
		{
			sq->assist(defPos);
		}
	}
}

void Commander::assistWorker(BaseAgent* worker)
{
	if (worker->getSquadID() != -1) return;
	if (!worker->getUnit()->isGatheringMinerals())
	{
		if (!worker->getUnit()->isGatheringGas())
		{
			return;
		}
	}

	//Find out who targets the worker
	TilePosition defPos = worker->getUnit()->getTilePosition();
	Unit* target = NULL;
	int bestScore = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			double dist = (*i)->getDistance(Position(defPos));
			if (dist <= 13 * 32)
			{
				//Found a target
				if ((*i)->getType().destroyScore() > bestScore)
				{
					target = (*i);
					bestScore = (*i)->getType().destroyScore();
				}
			}
		}
	}

	if (target != NULL)
	{
		//Broodwar->printf("Assisting building: Targeting enemy %s", target->getType().getName().c_str());
		defPos = target->getTilePosition();
	}
	
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		bool ok = true;
		if (sq->isExplorer()) ok = false;
		if (sq->isBunkerDefend()) ok = false;
		if (sq->isRush() && sq->isActive()) ok = false;

		if (ok)
		{
			sq->assist(defPos);
		}
	}
}

void Commander::forceAttack()
{
	TilePosition cGoal = getClosestEnemyBuilding(Broodwar->self()->getStartLocation());
	Broodwar->printf("[%d] Launch attack (%d,%d)", Broodwar->getFrameCount(), cGoal.x(), cGoal.y());
	if (cGoal.x() == -1)
	{
		return;
	}

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->isOffensive() || squads.at(i)->isSupport())
		{
			if (cGoal.x() >= 0)
			{
				squads.at(i)->forceActive();
				squads.at(i)->attack(cGoal);
			}
		}
	}

	currentState = ATTACK;
}

void Commander::finishBuild(BaseAgent* baseAgent)
{
	//First we must check if someone is repairing this building
	if(AgentManager::getInstance()->isAnyAgentRepairingThisAgent(baseAgent))
		return;

	BaseAgent* repUnit = AgentManager::getInstance()->findClosestFreeWorker(baseAgent->getUnit()->getTilePosition());
	if (repUnit != NULL)
	{
		repUnit->assignToFinishBuild(baseAgent->getUnit());
	}
}

bool Commander::checkUnfinishedBuildings()
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		// Double check so we don't get a dead agent. This could cause null pointers.
		if (agent != NULL && agent->isAlive() && agent->isBuilding() && agent->isDamaged())
		{
			Unit* builder = agent->getUnit()->getBuildUnit();
			if (builder == NULL || !builder->isConstructing())
			{
				finishBuild(agent);
			}
		}
	}
	return false;
}

void Commander::toggleBuildplanDebug()
{
	debug_bp = !debug_bp;
}

void Commander::toggleSquadsDebug()
{
	debug_sq = !debug_sq;
}

string Commander::format(string str)
{
	string res = str;

	string raceName = Broodwar->self()->getRace().getName();
	if (str.find(raceName) == 0)
	{
		int i = str.find(" ");
		res = str.substr(i + 1, str.length());
	}

	if (res == "Siege Tank Tank Mode") res = "Siege Tank";

	return res;
}

void Commander::printInfo()
{
	if (debug_sq)
	{
		int totLines = 0;
		for (int i = 0; i < (int)squads.size(); i++)
		{
			Squad* sq = squads.at(i);
			bool vis = true;
			if (sq->getTotalUnits() == 0) vis = false;
			if (sq->isBunkerDefend()) vis = false;
			if (sq->getPriority() == 1000 && !sq->isActive()) vis = false;

			if (vis)
			{
				totLines++;
			}	
		}
		if (totLines == 0) totLines++;

		Broodwar->drawBoxScreen(168,25,292,41+totLines*16,Colors::Black,true);
		if (currentState == DEFEND) Broodwar->drawTextScreen(170,25,"\x03Squads \x07(Defending)");
		if (currentState == ATTACK) Broodwar->drawTextScreen(170,25,"\x03Squads \x08(Attacking)");
		Broodwar->drawLineScreen(170,39,290,39,Colors::Orange);
		int no = 0;
		for (int i = 0; i < (int)squads.size(); i++)
		{
			Squad* sq = squads.at(i);
			bool vis = true;
			if (sq->getTotalUnits() == 0) vis = false;
			if (sq->isBunkerDefend()) vis = false;
			if (sq->getPriority() == 1000 && !sq->isActive()) vis = false;

			if (vis)
			{
				int cSize = sq->getSize();
				int totSize = sq->getTotalUnits();

				if (sq->isRequired())
				{
					if (cSize < totSize) Broodwar->drawTextScreen(170,41+no*16, "*SQ %d: \x18(%d/%d)", sq->getID(), sq->getSize(), sq->getTotalUnits());
					else Broodwar->drawTextScreen(170,41+no*16, "*SQ %d: \x07(%d/%d)", sq->getID(), sq->getSize(), sq->getTotalUnits());
					no++;
				}
				else
				{
					if (cSize < totSize) Broodwar->drawTextScreen(170,41+no*16, "SQ %d: \x18(%d/%d)", sq->getID(), sq->getSize(), sq->getTotalUnits());
					else Broodwar->drawTextScreen(170,41+no*16, "SQ %d: \x07(%d/%d)", sq->getID(), sq->getSize(), sq->getTotalUnits());
					no++;
				}
			}	
		}
		if (no == 0) no++;
		Broodwar->drawLineScreen(170,40+no*16,290,40+no*16,Colors::Orange);
	}

	if (debug_bp)
	{
		int totLines = (int)buildplan.size();
		if (totLines > 4) totLines = 4;
		if (totLines == 0) totLines = 1;
		
		Broodwar->drawBoxScreen(298,25,482,41+totLines*16,Colors::Black,true);
		Broodwar->drawTextScreen(300,25,"\x03Strategy Plan");
		Broodwar->drawLineScreen(300,39,480,39,Colors::Orange);
		int no = 0;

		int max = (int)buildplan.size();
		if (max > 4) max = 4;

		for (int i = 0; i < max; i++)
		{
			string name = "";
			if (buildplan.at(i).type == BuildplanEntry::BUILDING) name = buildplan.at(i).unittype.getName();
			if (buildplan.at(i).type == BuildplanEntry::UPGRADE) name = buildplan.at(i).upgradetype.getName();
			if (buildplan.at(i).type == BuildplanEntry::TECH) name = buildplan.at(i).techtype.getName();
			name = format(name);

			stringstream ss;
			ss << name;
			ss << " \x0F(@";
			ss << buildplan.at(i).supply;
			ss << ")";
			
			Broodwar->drawTextScreen(300,40+no*16, ss.str().c_str());
			no++;
		}
		if (no == 0) no++;
		Broodwar->drawLineScreen(300,40+no*16,480,40+no*16,Colors::Orange);
		Constructor::getInstance()->printInfo();
	}
}

int Commander::addBunkerSquad()
{

	/*
	Squad* bSquad = new Squad(100 + AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Bunker), Squad::BUNKER, "BunkerSquad", 5);
	bSquad->addSetup(UnitTypes::Terran_Marine, 4);
	squads.push_back(bSquad);

	//Try to fill from other squads.
	int added = 0;
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->isOffensive() || sq->isDefensive())
		{
			for (int i = 0; i < 4 - added; i++)
			{
				if (sq->hasUnits(UnitTypes::Terran_Marine, 1))
				{
					if (added < 4)
					{
						BaseAgent* ma = sq->removeMember(UnitTypes::Terran_Marine);
						if (ma != NULL)
					{
							added++;
							bSquad->addMember(ma);
							ma->clearGoal();
						}
					}
				}
			}
		}
	}*/

	return 0;
}
