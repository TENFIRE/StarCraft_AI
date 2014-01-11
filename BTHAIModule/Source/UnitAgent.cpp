#include "UnitAgent.h"
#include "NavigationAgent.h"
#include "TargetingAgent.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include <sstream>
#include <limits>

UnitAgent::UnitAgent()
{

}

UnitAgent::~UnitAgent()
{
	
}

UnitAgent::UnitAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("UnitAgent created (%s)", unit->getType().getName().c_str());
	dropped = 0;
	agentType = "UnitAgent";

	goal = TilePosition(-1, -1);

	infoUpdateFrame = 0;
	targetSwitchFrame = 0;
}

void UnitAgent::debug_showGoal()
{
	if (!isAlive()) return;
	if (unit->isLoaded()) return;
	if (unit->isBeingConstructed()) return;
	if (!unit->isCompleted()) return;
	
	if (goal.x() >= 0 && unit->isMoving())
	{
		Position a = unit->getPosition();
		Position b = Position(goal.x()*32+16, goal.y()*32+16);
		Broodwar->drawLineMap(a.x(),a.y(),b.x(),b.y(),Colors::Teal);
	}

	if (!unit->isIdle())
	{
		Unit* targ = unit->getOrderTarget();
		if (targ == NULL) unit->getTarget();

		if (targ != NULL)
		{
			Position a = unit->getPosition();
			Position b = targ->getPosition();

			if (targ->getPlayer()->isEnemy(Broodwar->self()))
			{
				if (targ->exists())
				{
					Broodwar->drawLineMap(a.x(),a.y(),b.x(),b.y(),Colors::Red);
				}
			}
			else
			{
				if (targ->exists())
				{
					Broodwar->drawLineMap(a.x(),a.y(),b.x(),b.y(),Colors::Green);
				}
			}
		}
	}

	if (unit->isBeingHealed())
	{
		Broodwar->drawCircleMap(unit->getPosition().x(), unit->getPosition().y(), 32, Colors::White, false);
	}

	if (unit->getType().isDetector())
	{
		double range = unit->getType().sightRange();
		int x = unit->getPosition().x();
		int y = unit->getPosition().y();
		Broodwar->drawCircleMap(x,y,(int)range, Colors::Red, false);
	}
}

void UnitAgent::computeActions()
{
	defensive = false;
	NavigationAgent::getInstance()->computeMove(this, goal, defensive);
	TargetingAgent::checkTarget(this);
}

int UnitAgent::enemyUnitsWithinRange(int maxRange)
{
	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			double dist = unit->getDistance((*i));
			if (dist <= maxRange)
			{
				eCnt++;
			}
		}
	}

	return eCnt;
}

int UnitAgent::enemyGroundUnitsWithinRange(int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if (!((*i)->getType().isFlyer()))
		{
			if ((*i)->exists())
			{
				double dist = unit->getDistance((*i));
				if (dist <= maxRange)
				{
					eCnt++;
				}
			}
		}
	}

	return eCnt;
}

int UnitAgent::enemySiegedTanksWithinRange(TilePosition center)
{
	int maxRange = 12 * 32 + 16;
	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getType().getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
			{
				double dist = (*i)->getDistance(Position(center));
				if (dist <= maxRange)
				{
					eCnt++;
				}
			}
		}
	}

	return eCnt;
}

int UnitAgent::enemyGroundAttackingUnitsWithinRange(TilePosition center, int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			if (!((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding()))
			{
				if (canAttack((*i)->getType(), unit->getType()))
				{
					double dist = (*i)->getDistance(Position(center));
					if (dist <= maxRange)
				{
						eCnt++;
					}
				}
			}
		}
	}

	return eCnt;
}

int UnitAgent::enemyAirUnitsWithinRange(int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			if ((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding())
			{
				double dist = unit->getDistance((*i));
				if (dist <= maxRange)
				{
					eCnt++;
				}
			}
		}	
	}

	return eCnt;
}

int UnitAgent::enemyAirToGroundUnitsWithinRange(int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			UnitType type = (*i)->getType();
			if (type.groundWeapon().targetsGround() || type.airWeapon().targetsGround())
			{
				if ((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding())
				{
					double dist = unit->getDistance((*i));
					if (dist <= maxRange)
				{
						eCnt++;
					}
				}
			}
		}	
	}

	return eCnt;
}

int UnitAgent::enemyAirAttackingUnitsWithinRange(TilePosition center, int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			if ((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding())
			{
				if (canAttack((*i)->getType(), unit->getType()))
				{
					double dist = (*i)->getDistance(Position(center));
					if (dist <= maxRange)
				{
						eCnt++;
					}
				}
			}
		}
		
	}

	return eCnt;
}

bool UnitAgent::useDefensiveMode()
{
	if (unit->getGroundWeaponCooldown() > 2 || unit->getAirWeaponCooldown() > 2)
	{
		return true;
	}
	return false;
}

int UnitAgent::enemyAttackingUnitsWithinRange()
{
	return enemyGroundAttackingUnitsWithinRange(unit->getTilePosition(), getGroundRange()) + enemyAirAttackingUnitsWithinRange(unit->getTilePosition(), getAirRange());
}

int UnitAgent::enemyAttackingUnitsWithinRange(int maxRange, TilePosition center)
{
	return enemyGroundAttackingUnitsWithinRange(center, maxRange) + enemyAirAttackingUnitsWithinRange(center, maxRange);
}

int UnitAgent::enemyAttackingUnitsWithinRange(UnitType type)
{
	return enemyGroundAttackingUnitsWithinRange(unit->getTilePosition(), getGroundRange(type)) + enemyAirAttackingUnitsWithinRange(unit->getTilePosition(), getAirRange(type));
}

Unit* UnitAgent::getClosestOrganicEnemy(int maxRange)
{
	Unit* enemy = NULL;
	double bestDist = DBL_MAX;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getType().isOrganic() && !(*i)->getType().isBuilding())
			{
				double cDist = unit->getDistance((*i));
				if (cDist < bestDist && cDist <= maxRange)
				{
					bestDist = cDist;
					enemy = (*i);
				}
			}
		}
	}

	return enemy;
}

Unit* UnitAgent::getClosestShieldedEnemy(int maxRange)
{
	Unit* enemy = NULL;
	double bestDist = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getShields() > 0)
			{
				double cDist = unit->getDistance((*i));
				if (bestDist < 0 || cDist < bestDist)
				{
					bestDist = cDist;
					enemy = (*i);
				}
			}
		}
	}

	return enemy;
}

Unit* UnitAgent::getClosestEnemyAirDefense(int maxRange)
{
	Unit* enemy = NULL;
	double bestDist = 100000;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			UnitType type = (*i)->getType();
			
			bool canAttackAir = false;
			if (type.isBuilding())
			{
				if (type.groundWeapon().targetsAir()) canAttackAir = true;
				if (type.airWeapon().targetsAir()) canAttackAir = true;
			}

			if (canAttackAir)
			{
				double cDist = unit->getDistance((*i));
				if (cDist < bestDist)
				{
					bestDist = cDist;
					enemy = (*i);
				}
			}
		}
	}

	if (bestDist >= 0 && bestDist <= maxRange)
	{
		return enemy;
	}
	else
	{
		return NULL;
	}
}

int UnitAgent::friendlyUnitsWithinRange()
{
	return friendlyUnitsWithinRange(192);
}

int UnitAgent::friendlyUnitsWithinRange(int maxRange)
{
	int fCnt = 0;
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isUnit() && !agent->isOfType(UnitTypes::Terran_Medic))
		{
			double dist = unit->getDistance(agent->getUnit());
			if (dist <= maxRange)
			{
				fCnt++;
			}
		}
	}
	return fCnt;
}

int UnitAgent::friendlyUnitsWithinRange(TilePosition tilePos, int maxRange)
{
	int fCnt = 0;
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
		BaseAgent* agent = agents.at(i);
		if (agent->isUnit() && !agent->isOfType(UnitTypes::Terran_Medic))
		{
			double dist = agent->getUnit()->getDistance(Position(tilePos));
			if (dist <= maxRange)
			{
				fCnt++;
			}
		}
	}
	return fCnt;
}

int UnitAgent::getGroundRange()
{
	return getGroundRange(unit->getType());
}

int UnitAgent::getGroundRange(UnitType type)
{
	WeaponType wep1 = type.groundWeapon();
	WeaponType wep2 = type.airWeapon();

	int maxRange = -1;
	if (wep1.targetsGround())
	{
		maxRange = wep1.maxRange();
	}
	if (wep2.targetsGround())
	{
		if (wep2.maxRange() > maxRange)
		{
			maxRange = wep2.maxRange();
		}
	}
	
	return maxRange;
}

int UnitAgent::getAirRange()
{
	return getAirRange(unit->getType());
}

int UnitAgent::getAirRange(UnitType type)
{
	WeaponType wep1 = type.groundWeapon();
	WeaponType wep2 = type.airWeapon();

	int maxRange = -1;
	if (wep1.targetsAir())
	{
		maxRange = wep1.maxRange();
	}
	if (wep2.targetsAir())
	{
		if (wep2.maxRange() > maxRange)
		{
			maxRange = wep2.maxRange();
		}
	}
	
	return maxRange;
}

bool UnitAgent::canAttack(UnitType attacker, UnitType target)
{
	if (!attacker.canAttack())
	{
		return false;
	}

	if (target.isFlyer() || target.isFlyingBuilding())
	{
		if (getAirRange(attacker) >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (getGroundRange(attacker) >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void UnitAgent::setGoal(TilePosition goal)
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


void UnitAgent::clearGoal()
{
	goal = TilePosition(-1, -1);
}

void UnitAgent::printInfo()
{
	int e = Broodwar->getFrameCount() - infoUpdateFrame;
	if (e >= infoUpdateTime || (sx == 0 && sy == 0))
	{
		infoUpdateFrame = Broodwar->getFrameCount();
		sx = unit->getPosition().x();
		sy = unit->getPosition().y();
	}	

	Broodwar->drawBoxMap(sx-2,sy,sx+242,sy+105,Colors::Black,true);
	Broodwar->drawTextMap(sx+4,sy,"\x03%s", format(unit->getType().getName()).c_str());
	Broodwar->drawLineMap(sx,sy+14,sx+240,sy+14,Colors::Green);

	Broodwar->drawTextMap(sx+2,sy+15,"Id: \x11%d", unitID);
	Broodwar->drawTextMap(sx+2,sy+30,"Position: \x11(%d,%d)", unit->getTilePosition().x(), unit->getTilePosition().y());
	Broodwar->drawTextMap(sx+2,sy+45,"Goal: \x11(%d,%d)", goal.x(), goal.y());
	Broodwar->drawTextMap(sx+2,sy+60,"Squad: \x11%d", squadID);
	
	int range = unit->getType().seekRange();
	if (unit->getType().sightRange() > range)
	{
		range = unit->getType().sightRange();
	}
	int enemyInRange = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		double dist = unit->getPosition().getDistance((*i)->getPosition());
		if (dist <= range)
		{
			enemyInRange++;
			break;
		}
	}

	Broodwar->drawTextMap(sx+2,sy+75,"Range: \x11%d", range);
	if (enemyInRange == 0) Broodwar->drawTextMap(sx+2,sy+90,"Enemies seen: \x11%d", enemyInRange);
	else Broodwar->drawTextMap(sx+2,sy+90,"Enemies seen: \x08%d", enemyInRange);

	string str = "\x07No";
	if (unit->isAttacking() || unit->isStartingAttack()) str = "\x08Yes";

	//Column two
	Broodwar->drawTextMap(sx+100,sy+15,"Attacking: %s", str.c_str());
	int nsy = sy+30;
	if (type.groundWeapon().targetsGround()) 
	{
		stringstream ss;
		if (unit->getGroundWeaponCooldown() == 0) ss << "\x07Ready";
		else
		{
			ss << "\x08";
			ss << unit->getGroundWeaponCooldown();
		}

		Broodwar->drawTextMap(sx+100,nsy,"Ground CD: %s", ss.str().c_str());
		nsy += 15;
	}

	if (type.airWeapon().targetsAir()) 
	{
		stringstream ss;
		if (unit->getAirWeaponCooldown() == 0) ss << "\x07Ready";
		else
		{
			ss << "\x08";
			ss << unit->getAirWeaponCooldown();
		}

		Broodwar->drawTextMap(sx+100,nsy,"Air CD: %s", ss.str().c_str());
		nsy += 15;
	}

	Unit* target = unit->getTarget();
	if (target == NULL) target = unit->getOrderTarget();
	str = "";
	if (target != NULL)
	{
		str = format(target->getType().getName());
	}
	Broodwar->drawTextMap(sx+100,nsy,"Target: \x11%s", str.c_str());
	nsy += 15;
	
	if (defensive) Broodwar->drawTextMap(sx+100,nsy,"Defensive: \x11Yes");
	else  Broodwar->drawTextMap(sx+100,nsy,"Defensive: \x11No");

	Broodwar->drawLineMap(sx,sy+104,sx+240,sy+104,Colors::Green);
}

bool UnitAgent::chargeShields()
{
	int cShields = unit->getShields();
	int maxShields = unit->getType().maxShields();

	if (cShields < maxShields)
	{
		//Shields are damaged
		BaseAgent* charger = AgentManager::getInstance()->getClosestAgent(unit->getTilePosition(), UnitTypes::Protoss_Shield_Battery);
		if (charger != NULL)
		{
			//Charger has energy
			if (charger->getUnit()->getEnergy() > 0)
			{
				double dist = charger->getUnit()->getTilePosition().getDistance(unit->getTilePosition());
				if (dist <= 15)
				{
					//We have charger nearby. Check if we have enemies around
					int eCnt = enemyAttackingUnitsWithinRange(12 * 32, unit->getTilePosition());
					if (eCnt == 0)
				{
						unit->rightClick(charger->getUnit());
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool UnitAgent::getDefensive()
{
	return defensive;
}

bool UnitAgent::canSwitchTarget()
{
	if (Broodwar->getFrameCount() - targetSwitchFrame >= 40)
	{
		return true;
	}
	return false;
}

void UnitAgent::setTargetSwitch()
{
	targetSwitchFrame = Broodwar->getFrameCount();
}
