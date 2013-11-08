#include "NavigationAgent.h"
#include "PFFunctions.h"
#include "AgentManager.h"
#include "Commander.h"
#include "Profiler.h"
#include "WalkTile.h"

bool NavigationAgent::instanceFlag = false;
NavigationAgent* NavigationAgent::instance = NULL;

NavigationAgent::NavigationAgent()
{
	checkRange = 5;
	mapW = Broodwar->mapWidth() * 4;
	mapH = Broodwar->mapHeight() * 4;
}

NavigationAgent::~NavigationAgent()
{
	instanceFlag = false;
	instance = NULL;
}

NavigationAgent* NavigationAgent::getInstance()
{
	if (!instanceFlag)
	{
		instance = new NavigationAgent();
		instanceFlag = true;
	}
	return instance;
}

void NavigationAgent::computeMove(BaseAgent* agent, TilePosition goal, bool defensive)
{
	double r = agent->getUnitType().seekRange();
	if (agent->getUnitType().sightRange() > r)
	{
		r = agent->getUnitType().sightRange();
	}

	bool enemyInRange = false;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		double dist = agent->getUnit()->getPosition().getDistance((*i)->getPosition());
		if (dist <= r)
		{
			enemyInRange = true;
			break;
		}
	}

	if (enemyInRange)
	{
		Profiler::getInstance()->start("PFmove");
		computePotentialFieldMove(agent, defensive);
		Profiler::getInstance()->end("PFmove");
	}
	else
	{
		Profiler::getInstance()->start("NormMove");
		computePathfindingMove(agent, goal);
		Profiler::getInstance()->end("NormMove");
	}
}

void NavigationAgent::computePotentialFieldMove(BaseAgent* agent, bool defensive)
{
	Unit* unit = agent->getUnit();

	if (unit->isSieged() || unit->isBurrowed() || unit->isLoaded())
	{
		return;
	}
	
	WalkTile unitWT = WalkTile(unit->getPosition());
	int wtX = unitWT.X();
	int wtY = unitWT.Y();

	float bestP = getAttackingUnitP(agent, unitWT, defensive);
	//bestP += PFFunctions::getGoalP(Position(unitX,unitY), goal);
	//bestP += PFFunctions::getTrailP(agent, unitX, unitY);
	bestP += PFFunctions::getTerrainP(agent, unitWT);

	float cP = 0;
	
	float startP = bestP;
    int bestX = wtX;
    int bestY = wtY;

	for (int cX = wtX - checkRange; cX <= wtX + checkRange; cX++)
	{
        for (int cY = wtY - checkRange; cY <= wtY + checkRange; cY++)
		{
			if (cX >= 0 && cY >= 0 && cX <= mapW && cY <= mapH)
			{
				WalkTile wt = WalkTile(cX, cY);
				cP = getAttackingUnitP(agent, wt, defensive);
				//cP += PFFunctions::getGoalP(Position(cX,cY), goal);
				//cP += PFFunctions::getTrailP(agent, cX, cY);
				cP += PFFunctions::getTerrainP(agent, wt);
				
                if (cP > bestP)
				{
					bestP = cP;
					bestX = cX;
					bestY = cY;
				}
            }
        }
    }
	
	if (bestX != wtX || bestY != wtY)
	{
		WalkTile wt = WalkTile(bestX, bestY);
		Position toMove = wt.getPosition();

		if (defensive)
		{
			//double d = toMove.getDistance(unit->getPosition());
			//if (agent->isOfType(UnitTypes::Protoss_Dragoon)) Broodwar->printf("PR rightclick move %d (%d,%d)->(%d,%d)", (int)d, unitX, unitY, bestX, bestY);
			unit->rightClick(toMove, true);
			unit->rightClick(toMove, false);
			unit->rightClick(toMove, true);
			unit->rightClick(toMove, false);
			unit->rightClick(toMove, true);
		}
		else
		{
			if (!unit->isAttacking() || !unit->isStartingAttack())
			{
				unit->attack(toMove);
			}
		}
		return;
    }
}

void NavigationAgent::computePotentialFieldDefendingMove(BaseAgent* agent)
{
	Unit* unit = agent->getUnit();

	if (unit->isSieged() || unit->isBurrowed() || unit->isLoaded())
	{
		return;
	}
	
	WalkTile unitWT = WalkTile(unit->getPosition());
	int wtX = unitWT.X();
	int wtY = unitWT.Y();

	float bestP = getDefendingUnitP(agent, unitWT);
	float cP = 0;
	
	float startP = bestP;
    int bestX = wtX;
    int bestY = wtY;

	for (int cX = wtX - checkRange; cX <= wtX + checkRange; cX++)
	{
        for (int cY = wtY - checkRange; cY <= wtY + checkRange; cY++)
		{
			if (cX >= 0 && cY >= 0 && cX <= mapW && cY <= mapH)
			{
				WalkTile wt = WalkTile(cX, cY);
				cP = getDefendingUnitP(agent, wt);
				
                if (cP > bestP)
				{
					bestP = cP;
					bestX = cX;
					bestY = cY;
				}
            }
        }
    }
	
	if (bestX != wtX || bestY != wtY)
	{
		WalkTile wt = WalkTile(bestX, bestY);
		Position toMove = wt.getPosition();

		unit->attack(toMove);
    }
}

void NavigationAgent::computePathfindingMove(BaseAgent* agent, TilePosition goal)
{
	TilePosition checkpoint = goal;
	if (agent->getSquadID() >= 0)
	{
		Squad* sq = Commander::getInstance()->getSquad(agent->getSquadID());
		if (sq != NULL)
		{
			checkpoint = sq->nextMovePosition();
			if (agent->isOfType(UnitTypes::Terran_SCV))
			{
				checkpoint = sq->nextFollowMovePosition();
				agent->setGoal(checkpoint);
			}
		}
	}
	
	if (goal.x() != -1)
	{
		moveToGoal(agent, checkpoint, goal);
	}
}

void NavigationAgent::displayPF(BaseAgent* agent)
{
	Unit* unit = agent->getUnit();
	if (unit->isBeingConstructed()) return;

	//PF
	WalkTile w = WalkTile(agent->getUnit()->getPosition());
	int tileX = w.X();
	int tileY = w.Y();
	int range = 10*3;

	for (int cTileX = tileX - range; cTileX < tileX + range; cTileX+=3)
	{
        for (int cTileY = tileY - range; cTileY < tileY + range; cTileY+=3)
		{
            if (cTileX >= 0 && cTileY >= 0 && cTileX < mapW && cTileY < mapH)
			{
				WalkTile wt = WalkTile(cTileX+1, cTileY+1);
				float p = getAttackingUnitP(agent, wt, agent->getDefensive());
				//cP += PFFunctions::getGoalP(Position(cX,cY), goal);
				//cP += PFFunctions::getTrailP(agent, cX, cY);
				p += PFFunctions::getTerrainP(agent, wt);
					
				//print box
				if (p > -950)
				{
					Position pos = wt.getPosition();
					Broodwar->drawBoxMap(pos.x()-8,pos.y()-8,pos.x()+8,pos.y()+8,getColor(p),true);
				}
            }
        }
    }
}

Color NavigationAgent::getColor(float p)
{
	if (p >= 0)
	{
		int v = (int)(p * 3);
		int halfV = (int)(p * 0.6);

		if (v > 255) v = 255;
		if (halfV > 255) halfV = 255;

		return Color(halfV, halfV, v);
	}
	else
	{
		p = -p;
		int v = (int)(p * 1.6);
		
		int v1 = 255 - v;
		if (v1 <= 0) v1 = 40;
		int halfV1 = v1 * 0.6;
		
		return Color(v1, halfV1, halfV1);
	}
}

bool NavigationAgent::moveToGoal(BaseAgent* agent,  TilePosition checkpoint, TilePosition goal)
{
	if (checkpoint.x() == -1 || goal.x() == -1) return false;
	Unit* unit = agent->getUnit();

	if (unit->isStartingAttack() || unit->isAttacking())
	{
		return false;
	}

	Position toReach = Position(checkpoint.x()*32+16, checkpoint.y()*32+16);
	double distToReach = toReach.getDistance(unit->getPosition());

	int engageDist = Broodwar->self()->groundWeaponMaxRange(unit->getType()) * 0.5;
	if (engageDist == 0) engageDist = Broodwar->self()->airWeaponMaxRange(unit->getType()) * 0.5;
	if (engageDist < 2*32) engageDist = 2*32;

	//Explorer units shall have
	//less engage dist to avoid getting
	//stuck at waypoints.
	Squad* sq = Commander::getInstance()->getSquad(agent->getSquadID());
	if (sq != NULL)
	{
		if (sq->isExplorer())
		{
			engageDist = 32;
		}
		else
		{
			//Add additional distance to avoid clogging
			//choke points.
			if (!sq->isActive())
			{
				engageDist += 4*32;
			}
		}
	}

	if (distToReach <= engageDist)
	{
		//Dont stop close to chokepoints
		TilePosition tp = unit->getTilePosition();
		Chokepoint* cp = getNearestChokepoint(tp);
		double d = tp.getDistance(TilePosition(cp->getCenter()));
		if (d > 4)
		{
			if (unit->isMoving()) unit->stop();
			return true;
		}
	}

	int squadID = agent->getSquadID();
	if (squadID != -1)
	{
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			if (sq->isAttacking())
			{
				//Squad is attacking. Don't stop
				//at checkpoints.
				toReach = Position(goal.x()*32+16,goal.y()*32+16);
			}
		}
	}
	//Move
	if (!unit->isMoving()) return unit->attack(toReach);
	else return true;
}

float NavigationAgent::getAttackingUnitP(BaseAgent* agent, WalkTile wp, bool defensive)
{
	float p = 0;
	
	//Enemy Units
	float p_off = 0;
	float p_def = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		float dist = PFFunctions::getDistance(wp, (*i));
		float ptmp = PFFunctions::calcOffensiveUnitP(dist, agent->getUnit(), (*i));
		p_def += PFFunctions::calcDefensiveUnitP(dist, agent->getUnit(), (*i));

		if (ptmp > p_off)
		{
			p_off = ptmp;
		}
	}

	if (!defensive) p = p_off;

	if (defensive || p_off == 0)
	{
		p += p_def;
	}
	
	//Own Units
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			float dist = PFFunctions::getDistance(wp, agents.at(i)->getUnit());
			float ptmp = PFFunctions::calcOwnUnitP(dist, wp, agent->getUnit(), agents.at(i)->getUnit());
			p += ptmp;
		}
	}

	//Neutral Units
	for(set<Unit*>::const_iterator i=Broodwar->getNeutralUnits().begin();i!=Broodwar->getNeutralUnits().end();i++)
	{
		if ((*i)->getType().getID() == UnitTypes::Terran_Vulture_Spider_Mine.getID())
		{
			WalkTile w2 = WalkTile((*i));
			float dist = PFFunctions::getDistance(wp, (*i));
			if (dist <= 2)
			{
				p -= 50.0;
			}
		}
	}

	return p;
}

float NavigationAgent::getDefendingUnitP(BaseAgent* agent, WalkTile wp)
{
	float p = 0;
	
	p += PFFunctions::getGoalP(agent, wp);
	p += PFFunctions::getTerrainP(agent, wp);

	//Own Units
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			float dist = PFFunctions::getDistance(wp, agents.at(i)->getUnit());
			float ptmp = PFFunctions::calcOwnUnitP(dist, wp, agent->getUnit(), agents.at(i)->getUnit());
			p += ptmp;
		}
	}

	return p;
}
