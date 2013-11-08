#include "BaseAgent.h"
#include "Constructor.h"
#include "AgentManager.h"
#include "ResourceManager.h"
#include "ExplorationManager.h"

BaseAgent::BaseAgent()
{
	alive = true;
	squadID = -1;
	type = UnitTypes::Unknown;
	lastActionFrame = 0;
	goal = TilePosition(-1, -1);

	infoUpdateFrame = 0;
	infoUpdateTime = 20;
	sx = 0;
	sy = 0;
}

BaseAgent::BaseAgent(Unit* mUnit)
{
	unit = mUnit;
	unitID = unit->getID();
	type = unit->getType();
	alive = true;
	squadID = -1;
	lastActionFrame = 0;
	goal = TilePosition(-1, -1);
	agentType = "BaseAgent";
}

BaseAgent::~BaseAgent()
{
	
}

string BaseAgent::getTypeName()
{
	return agentType;
}

void BaseAgent::printInfo()
{

}

int BaseAgent::getUnitID()
{
	return unitID;
}

string BaseAgent::format(string str)
{
	string res = str;

	int i = str.find(" ");
	if (i >= 0)
	{
		res = str.substr(i + 1, str.length());
	}

	if (res == "Siege Tank Tank Mode") res = "Siege Tank";
	if (res == "Siege Tank Siege Mode") res = "Siege Tank (sieged)";

	return res;
}

UnitType BaseAgent::getUnitType()
{
	return type;
}

Unit* BaseAgent::getUnit()
{
	return unit;
}

bool BaseAgent::matches(Unit *mUnit)
{
	if (isAlive())
	{
		if (mUnit->getID() == unitID)
		{
			return true;
		}
	}
	return false;
}

bool BaseAgent::isOfType(UnitType type)
{
	if (unit->getType().getID() == type.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isOfType(Unit* mUnit, UnitType type)
{
	if (mUnit->getType().getID() == type.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isOfType(UnitType mType, UnitType toCheckType)
{
	if (mType.getID() == toCheckType.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::canBuild(UnitType type)
{
	//1. Check if building is being constructed
	if (unit->isBeingConstructed())
	{
		return false;
	}

	//2. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//3. All is clear.
	return true;
}

bool BaseAgent::isBuilding()
{
	if (unit->getType().isBuilding())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isWorker()
{
	if (unit->getType().isWorker())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isUnit()
{
	if (unit->getType().isBuilding() || unit->getType().isWorker() || unit->getType().isAddon())
	{
		return false;
	}
	return true;
}

bool BaseAgent::isUnderAttack()
{
	if (unit == NULL) return false;
	if (!unit->exists()) return false;

	if (unit->isAttacking()) return true;
	if (unit->isStartingAttack()) return true;

	double r = unit->getType().seekRange();
	if (unit->getType().sightRange() > r)
	{
		r = unit->getType().sightRange();
	}

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		double dist = unit->getPosition().getDistance((*i)->getPosition());
		if (dist <= r)
		{
			return true;
		}
	}

	return false;
}

void BaseAgent::destroyed()
{
	alive = false;
}

bool BaseAgent::isAlive()
{
	return alive;
}

bool BaseAgent::isDamaged()
{
	if (unit->isBeingConstructed()) return false;
	if (unit->getRemainingBuildTime() > 0) return false;

	if (unit->getHitPoints() < unit->getType().maxHitPoints())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isDetectorWithinRange(TilePosition pos, int range)
{
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->getType().isDetector())
		{
			double dist = (*i)->getDistance(Position(pos));
			if (dist <= range)
			{
				return true;
			}
		}
	}
	return false;
}

bool BaseAgent::doScannerSweep(TilePosition pos)
{
	if (!Constructor::isTerran())
	{
		return false;
	}

	if (!type.getID() == UnitTypes::Terran_Comsat_Station.getID())
	{
		return false;
	}

	if (unit->getEnergy() >= 50)
	{
		if (unit->useTech(TechTypes::Scanner_Sweep, Position(pos)))
		{
			Broodwar->printf("SCAN CLOAKED ENEMY");
			return true;
		}
	}

	return false;
}

bool BaseAgent::doEnsnare(TilePosition pos)
{
	if (!Constructor::isZerg())
	{
		return false;
	}
	if (!Broodwar->self()->hasResearched(TechTypes::Ensnare))
	{
		return false;
	}

	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->isOfType(UnitTypes::Zerg_Queen))
		{
			if (agent->getUnit()->getEnergy() >= 75)
			{
				agent->getUnit()->useTech(TechTypes::Ensnare, Position(pos));
				return true;
			}
		}
	}

	return false;
}

void BaseAgent::setSquadID(int id)
{
	squadID = id;
}

int BaseAgent::getSquadID()
{
	return squadID;
}

void BaseAgent::setActionFrame()
{
	lastActionFrame = Broodwar->getFrameCount();
}

int BaseAgent::getLastActionFrame()
{
	return lastActionFrame;
}

bool BaseAgent::canAttack(Unit* target)
{
	return canAttack(target->getType());
}

bool BaseAgent::canAttack(UnitType type)
{
	if (unit->getType().isWorker()) return false;

	if (!type.isFlyer())
	{
		if (unit->getType().groundWeapon().targetsGround()) return true;
		if (unit->getType().airWeapon().targetsGround()) return true;
	}
	else
	{
		if (unit->getType().groundWeapon().targetsAir()) return true;
		if (unit->getType().airWeapon().targetsAir()) return true;
	}
	return false;
}

bool BaseAgent::enemyUnitsVisible()
{
	double r = unit->getType().seekRange();
	if (unit->getType().sightRange() > r)
	{
		r = unit->getType().sightRange();
	}

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		double dist = unit->getPosition().getDistance((*i)->getPosition());
		if (dist <= r)
		{
			return true;
		}
	}
	return false;
}

void BaseAgent::setGoal(TilePosition goal)
{
	if (unit->getType().isFlyer() || unit->getType().isFlyingBuilding())
	{
		//Flyers, can always move to goals.
		this->goal = goal;
	}
	else
	{
		//Ground units, check if we can reach goal.
		if (ExplorationManager::canReach(this, goal))
		{
			this->goal = goal;
		}
	}
}

void BaseAgent::clearGoal()
{
	goal = TilePosition(-1, -1);
}

TilePosition BaseAgent::getGoal()
{
	return goal;
}

void BaseAgent::addTrailPosition(WalkTile wt)
{
	//Check if position already is in trail
	if (trail.size() > 0)
	{
		WalkTile lwt = trail.at(trail.size() - 1);
		if (lwt.X() == wt.X() && lwt.Y() == wt.Y()) return;
	}

	trail.push_back(wt);
	if (trail.size() > 20)
	{
		trail.erase(trail.begin());
	}
}

vector<WalkTile> BaseAgent::getTrail()
{
	return trail;
}

bool BaseAgent::getDefensive()
{
	return false;
}
