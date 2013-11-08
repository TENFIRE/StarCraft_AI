#include "TargetingAgent.h"
#include "UnitAgent.h"

bool TargetingAgent::canAttack(UnitType type)
{
	if (type.isBuilding())
	{
		if (type.canAttack()) return true;
		return false;
	}
	if (type.isAddon())
	{
		return false;
	}
	if (type.isWorker())
	{
		return false;
	}
	return true;
}

int TargetingAgent::getNoAttackers(BaseAgent* agent)
{
	int cnt = 0;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if (canAttack((*i)->getType()))
		{
			int enemyMSD = 0;
			if (agent->getUnitType().isFlyer())
			{
				enemyMSD = (*i)->getType().airWeapon().maxRange();	
			}
			else
			{
				enemyMSD = (*i)->getType().groundWeapon().maxRange();	
			}

			double d = agent->getUnit()->getPosition().getDistance((*i)->getPosition());
			if (d <= enemyMSD)
			{
				cnt++;
			}
		}
	}

	return cnt;
}

void TargetingAgent::checkTarget(BaseAgent* agent)
{
	Unit* cTarget = agent->getUnit()->getTarget();
	if (cTarget == NULL)
	{
		cTarget = agent->getUnit()->getOrderTarget();
	}

	if (cTarget != NULL)
	{
		Unit* pTarget = findTarget(agent);
		if (pTarget != NULL && pTarget->getPlayer()->isEnemy(Broodwar->self()))
		{
			if (pTarget->getType().getID() != cTarget->getType().getID())
			{
				//Show debug info for selected unit, or first
				//in list if mulitple units are selected.
				set<Unit*> units = Broodwar->getSelectedUnits();
				if ((int)units.size() > 0) 
				{
					int unitID = (*units.begin())->getID();
					if (agent->getUnitID() == unitID)
					{
						int uX = agent->getUnit()->getPosition().x();
						int uY = agent->getUnit()->getPosition().y();
						int tX = pTarget->getPosition().x();
						int tY = pTarget->getPosition().y();
						Broodwar->drawLineMap(uX,uY,tX,tY,Colors::Yellow);
					}
				}
				UnitAgent* ua = (UnitAgent*)agent;
				if (ua->canSwitchTarget())
				{
					agent->getUnit()->attack(pTarget, true);
					agent->getUnit()->rightClick(pTarget, true);
				}
			}
		}
	}
}

bool TargetingAgent::isHighprioTarget(UnitType type)
{
	if (type.getID() == UnitTypes::Terran_Bunker.getID()) return true;
	if (type.getID() == UnitTypes::Terran_Battlecruiser.getID()) return true;
	if (type.getID() == UnitTypes::Terran_Missile_Turret.getID()) return true;
	
	if (type.getID() == UnitTypes::Protoss_Carrier.getID()) return true;
	if (type.getID() == UnitTypes::Protoss_Photon_Cannon.getID()) return true;
	if (type.getID() == UnitTypes::Protoss_Archon.getID()) return true;

	if (type.getID() == UnitTypes::Zerg_Sunken_Colony.getID()) return true;
	if (type.getID() == UnitTypes::Zerg_Spore_Colony.getID()) return true;
	if (type.getID() == UnitTypes::Zerg_Ultralisk.getID()) return true;

	return false;
}

Unit* TargetingAgent::findHighprioTarget(BaseAgent* agent, int maxDist, bool targetsAir, bool targetsGround)
{
	Unit* target = NULL;
	Position cPos = agent->getUnit()->getPosition();
	int bestTargetScore = -10000;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			UnitType t = (*i)->getType();
			bool targets = isHighprioTarget(t);
			if (t.isFlyer() && !targetsAir) targets = false;
			if (!t.isFlyer() && !targetsGround) targets = false;
			
			if (targets)
			{
				double dist = cPos.getDistance((*i)->getPosition());
				if (dist <= (double)maxDist)
				{
					if (t.destroyScore() > bestTargetScore)
					{
						target = (*i);
						bestTargetScore = t.destroyScore();
					}
				}
			}
		}
	}

	return target;
}

Unit* TargetingAgent::findTarget(BaseAgent* agent)
{
	//Check if the agent targets ground and/or air
	bool targetsGround = false;
	if (agent->getUnitType().groundWeapon().targetsGround()) targetsGround = true;
	if (agent->getUnitType().airWeapon().targetsGround()) targetsGround = true;

	bool targetsAir = false;
	if (agent->getUnitType().groundWeapon().targetsAir()) targetsAir = true;
	if (agent->getUnitType().airWeapon().targetsAir()) targetsAir = true;

	//Check max range
	int range = agent->getUnitType().seekRange();
	if (agent->getUnitType().sightRange() > range)
	{
		range = agent->getUnitType().sightRange();
	}
	range *= 1.5;

	if (agent->getUnit()->isSieged())
	{
		range = Broodwar->self()->groundWeaponMaxRange(agent->getUnit()->getType());
	}

	//Iterate through enemies to select a target
	int bestTargetScore = -10000;
	Unit* target = NULL;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		UnitType t = (*i)->getType();

		bool canAttack = false;
		if (!t.isFlyer() && targetsGround) canAttack = true;
		if ((t.isFlyer() || (*i)->isLifted()) && targetsAir) canAttack = true;
		if ((*i)->isCloaked() && !(*i)->isDetected())
		{
			canAttack = false;
			handleCloakedUnit((*i));
		}
		if ((*i)->isBurrowed() && !(*i)->isDetected())
		{
			canAttack = false;
			handleCloakedUnit((*i));
		}

		if (canAttack)
		{
			int d = (int)agent->getUnit()->getDistance((*i));
			if (d <= range)
			{
				double mod = getTargetModifier(agent->getUnit()->getType(), t);
				int cScore = (int)((double)t.destroyScore() * mod);
				if ((*i)->getHitPoints() < (*i)->getInitialHitPoints())
				{
					//Prioritize damaged targets
					cScore++;
				}

				if (cScore > bestTargetScore)
				{
					bestTargetScore = cScore;
					target = (*i);
				}
			}
		}
	}

	return target;
}

double TargetingAgent::getTargetModifier(UnitType attacker, UnitType target)
{
	//Non-attacking buildings
	if (target.isBuilding() && !target.canAttack() && !target.getID() == UnitTypes::Terran_Bunker.getID())
	{
		return 0.05;
	}

	//Terran Goliath prefer air targets
	if (attacker.getID() == UnitTypes::Terran_Goliath.getID())
	{
		if (target.isFlyer()) return 2;
	}

	//Siege Tanks prefer to take out enemy defense buildings
	if (attacker.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
	{
		if (target.isBuilding() && target.canAttack()) return 1.5;
		if (target.getID() == UnitTypes::Terran_Bunker.getID()) return 1.5;
	}

	//Siege Tanks are nasty and have high prio to be killed.
	if (target.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
	{
		return 1.5;
	}

	//Prio to take out detectors when having cloaking units
	if (isCloakingUnit(attacker) && target.isDetector())
	{
		return 2;
	}

	if (attacker.isFlyer() && !target.airWeapon().targetsAir())
	{
		//Target cannot attack back. Set to low prio
		return 0.1;
	}

	if (!attacker.isFlyer() && !target.groundWeapon().targetsGround())
	{
		//Target cannot attack back. Set to low prio
		return 0.1;
	}
	
	return 1; //Default: No modifier
}

void TargetingAgent::handleCloakedUnit(Unit* unit)
{
	//Terran: Cloaked units are handled by ComSat agent
	
	//Add code for handling cloaked units here.
}

bool TargetingAgent::isCloakingUnit(UnitType type)
{
	if (type.isCloakable()) return true;
	return false;
}
