#include "Squad.h"
#include "UnitAgent.h"
#include "ExplorationManager.h"
#include "Commander.h"
#include "Constructor.h"
#include "Pathfinder.h"
#include "Profiler.h"

Squad::Squad()
{

}

Squad::Squad(int mId, int mType, string mName, int mPriority)
{
	this->id = mId;
	this->type = mType;
	this->moveType = AIR;
	this->name = mName;
	this->priority = mPriority;
	morphs = UnitTypes::Unknown;
	activePriority = priority;
	active = false;
	required = false;
	goal = TilePosition(-1, -1);
	goalSetFrame = 0;
	arrivedFrame = -1;
	currentState = STATE_DEFEND;
	buildup = false;
}

string Squad::getName()
{
	return name;
}

UnitType Squad::morphsTo()
{
	return morphs;
}

void Squad::setMorphsTo(UnitType type)
{
	morphs = type;
}

int Squad::getID()
{
	return id;
}

bool Squad::isRequired()
{
	return required;
}

void Squad::setRequired(bool mRequired)
{
	required = mRequired;
}

void Squad::setBuildup(bool mBuildup)
{
	buildup = mBuildup;
}

int Squad::getPriority()
{
	return priority;
}

void Squad::setPriority(int mPriority)
{
	priority = mPriority;
}

void Squad::setActivePriority(int mPriority)
{
	activePriority = mPriority;
}

bool Squad::isActive()
{
	return active;
}

void Squad::forceActive()
{
	int noAlive = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			noAlive++;
		}
	}

	if (noAlive > 0)
	{
		active = true;
	}
}

int Squad::size()
{
	int no = 0;
	for(int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			no++;
		}
	}
	return no;
}

int Squad::maxSize()
{
	int no = 0;
	for (int i = 0; i < (int)setup.size(); i++)
	{
		no += setup.at(i).no;
	}
	return no;
}

void Squad::addSetup(UnitType type, int no)
{
	//First, check if we have the setup already
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).type.getID() == type.getID())
		{
			//Found, increase the amount
			setup.at(i).no += no;
			return;
		}
	}

	//Not found, add as new
	UnitSetup us;
	us.type = type;
	us.no = no;
	us.current = 0;
	setup.push_back(us);

	if (!type.isFlyer())
	{
		moveType = GROUND;
	}
}

void Squad::removeSetup(UnitType type, int no)
{
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).type.getID() == type.getID())
		{
			//Found, reduce the amount
			setup.at(i).no -= no;
			if (setup.at(i).no < 0) setup.at(i).no = 0;
			int toRemove = setup.at(i).current - setup.at(i).no;
			for (int j = 0; j < toRemove; j++)
			{
				removeMember(setup.at(i).type);
			}
			return;
		}
	}
}

void Squad::debug_showGoal()
{
	if (isBunkerDefend()) return;

	if (size() > 0 && goal.x() >= 0)
	{
		Position a = Position(goal.x()*32+16,goal.y()*32+16);
		
		Broodwar->drawCircleMap(a.x()-3, a.y()-3, 6, Colors::Grey, true);
		Broodwar->drawTextMap(a.x() - 20, a.y() - 5, "\x03SQ %d", id);
	}
}

void Squad::computeActions()
{
	if (!active)
	{
		if (isFull() && !buildup)
		{
			active = true;
		}
	}

	if (active)
	{
		if (activePriority != priority)
		{
			priority = activePriority;
		}
	}

	checkAttack();
}

bool Squad::isAttacking()
{
	if (isExplorer()) return false;
	if (isBunkerDefend()) return false;

	for(int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->getUnit()->isAttacking()) return true;
			if (agents.at(i)->getUnit()->isStartingAttack()) return true;
		}
	}
	
	return false;
}

void Squad::checkAttack()
{
	if (isExplorer()) return;
	if (isBunkerDefend()) return;

	Unit* target = findTarget();
		
	if (target != NULL)
	{
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* agent = agents.at(i);
			if (agent->isAlive() && agent->getUnit()->exists() && agent->canAttack(target) && !(agent->getUnit()->isAttacking() || agent->getUnit()->isStartingAttack()))
			{
				if (!agent->getUnit()->isBurrowed())
				{
					agent->getUnit()->attack(target);
				}
			}
		}
	}
}

Unit* Squad::findTarget()
{
	if (agents.size() == 0) return NULL;

	//Enemy units
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		int maxRange = 400;//400 = range of sieged Siege Tanks (384), plus a half tile (16).
		
		if (agent->isAlive())
		{
			for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
			{
				if ((*i)->exists())
				{
					double dist = agent->getUnit()->getDistance((*i));
					if (dist <= maxRange)
					{
						return (*i);
					}
				}
			}
		}
	}

	return NULL;
}

bool Squad::isUnderAttack()
{
	if (isBunkerDefend())
	{
		return false;
	}

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->getUnit()->isAttacking()) return true;
			if (agents.at(i)->getUnit()->isStartingAttack()) return true;
		}
	}
	return false;
}

bool Squad::needUnit(UnitType type)
{
	//1. Check if prio is set to Inactive squad.
	if (priority >= 1000)
	{	
		return false;
	}

	int noCreated = 1;
	if (Constructor::isZerg())
	{
		if (type.isTwoUnitsInOneEgg())
		{
			noCreated = 2;
		}
	}
	
	//2. Check setup
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(type))
		{
			//Found a matching setup, see if there is room
			if (setup.at(i).current + Constructor::getInstance()->noInProduction(type) + noCreated <= setup.at(i).no)
			{
				return true;
			}
		}
	}

	return false;
}

vector<BaseAgent*> Squad::getMembers()
{
	return agents;
}

bool Squad::addMember(BaseAgent* agent)
{
	if (priority >= 1000)
	{
		//Check if prio is above Inactive squad.
		return false;
	}

	//Step 1. Check if the agent already is in the squad
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnitID() == agent->getUnitID())
		{
			//Remove it, and add again to update the types.
			//Needed for morphing units like Lurkers.
			removeMember(agent);
		}
	}

	//Step 2. Check if we have room for this type of agent.
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(agent->getUnitType()))
		{
			//Found a matching setup, see if there is room
			if (setup.at(i).current < setup.at(i).no)
			{
				//Yes we have, add it to the squad
				agents.push_back(agent);
				agent->setSquadID(id);
				setup.at(i).current++;
				
				if (goal.x() >= 0)
				{
					agent->setGoal(goal);
				}

				return true;
			}
		}
	}

	return false;
}

void Squad::printInfo()
{
	int sx = 440;
	int sy = 30;
	int w = 180;
	int h = 120+15*setup.size();

	Broodwar->drawBoxScreen(sx-2,sy,sx+w+2,sy+h,Colors::Black,true);
	Broodwar->drawTextScreen(sx+4,sy,"\x03%s", name.c_str());
	Broodwar->drawLineScreen(sx,sy+14,sx+w,sy+14,Colors::Orange);

	Broodwar->drawTextScreen(sx+2,sy+15,"Id: \x11%d", id);
	Broodwar->drawTextScreen(sx+2,sy+30,"Goal: \x11(%d,%d)", goal.x(), goal.y());
	
	string str1 = "Ground ";
	if (isAir()) str1 = "Air ";
	if ((int)setup.size() == 0) str1 = "";

	string str2 = "";
	if (isOffensive()) str2 = "Offensive";
	if (isDefensive()) str2 = "Defensive";
	if (isBunkerDefend()) str2 = "Bunker";
	if (isExplorer()) str2 = "Explorer";
	if (isRush()) str2 = "Rush";

	Broodwar->drawTextScreen(sx+2,sy+45,"Type: \x11%s%s", str1.c_str(), str2.c_str());
	Broodwar->drawTextScreen(sx+2,sy+60,"Priority: \x11%d", activePriority);

	if (required) Broodwar->drawTextScreen(sx+2,sy+75,"Required: \x07Yes");
	else Broodwar->drawTextScreen(sx+2,sy+75,"Required: \x18No");

	if (isFull()) Broodwar->drawTextScreen(sx+2,sy+90,"Full: \x07Yes");
	else Broodwar->drawTextScreen(sx+2,sy+90,"Full: \x18No");

	if (isActive()) Broodwar->drawTextScreen(sx+2,sy+105,"Active: \x07Yes");
	else Broodwar->drawTextScreen(sx+2,sy+105,"Active: \x18No");

	Broodwar->drawLineScreen(sx,sy+119,sx+w,sy+119,Colors::Orange);
	int no = 0;
	for (int i = 0; i < (int)setup.size(); i++)
	{
		string name = Commander::format(setup.at(i).type.getName());
		Broodwar->drawTextScreen(sx+2,sy+120+15*no,"%s \x11(%d/%d)", name.c_str(), setup.at(i).current, setup.at(i).no);
		no++;
	}
	Broodwar->drawLineScreen(sx,sy+119+15*no,sx+w,sy+119+15*no,Colors::Orange);
}

bool Squad::isFull()
{
	if ((int)setup.size() == 0) return false;

	//1. Check setup
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).current < setup.at(i).no)
		{
			return false;
		}
	}

	//2. Check that all units are alive and ready
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnit()->exists())
		{
			if (!agents.at(i)->isAlive()) return false;
			if (agents.at(i)->getUnit()->isBeingConstructed()) return false;
		}
	}

	//3. Check if some morphing is needed
	if (morphs.getID() != UnitTypes::Unknown.getID())
	{
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (morphs.getID() == UnitTypes::Zerg_Lurker.getID() && agents.at(i)->isOfType(UnitTypes::Zerg_Hydralisk))
			{
				return false;
			}
			if (morphs.getID() == UnitTypes::Zerg_Devourer.getID() && agents.at(i)->isOfType(UnitTypes::Zerg_Mutalisk))
			{
				return false;
			}
			if (morphs.getID() == UnitTypes::Zerg_Guardian.getID() && agents.at(i)->isOfType(UnitTypes::Zerg_Mutalisk))
			{
				return false;
			}
		}
	}

	return true;
}

void Squad::removeMember(BaseAgent* agent)
{
	//Step 1. Remove the agent instance
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnitID() == agent->getUnitID())
		{
			agents.at(i)->setSquadID(-1);
			agents.at(i)->assignToDefend();
			agents.erase(agents.begin() + i);
			break;
		}
	}

	//Step 2. Update the setup list
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(agent->getUnitType()))
		{
			setup.at(i).current--;
		}
	}

	//Step 3. If Explorer, set destination as explored (to avoid being killed at the same
	//place over and over again).
	if (isExplorer())
	{
		TilePosition goal = agent->getGoal();
		if (goal.x() >= 0)
		{
			ExplorationManager::getInstance()->setExplored(goal);
		}
	}

	//See if squad should be set to inactive (due to too many dead units)
	if (active)
	{
		int noAlive = 0;
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (agents.at(i)->isAlive())
			{
				noAlive++;
			}
		}

		if (noAlive <= maxSize() / 10)
		{
			active = false;
		}
	}
}

void Squad::disband()
{
	//Remove setup
	for (int i = 0; i < (int)setup.size(); i++)
	{
		setup.at(i).no = 0;
		setup.at(i).current = 0;
	}

	//Remove agents
	/*for (int i = 0; i < (int)agents.size(); i++)
	{
		agents.at(i)->setSquadID(-1);
		agents.at(i)->assignToDefend();
	}*/
	agents.clear();
}

BaseAgent* Squad::removeMember(UnitType type)
{
	BaseAgent* agent = NULL;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (UnitSetup::equals(agents.at(i)->getUnitType(), type))
		{
			agent = agents.at(i);
			break;
		}
	}

	if (agent != NULL)
	{
		removeMember(agent);
	}

	return agent;
}

void Squad::defend(TilePosition mGoal)
{
	if (mGoal.x() == -1 || mGoal.y() == -1) return;

	if (currentState != STATE_DEFEND)
	{
		if (currentState == STATE_ASSIST && !isUnderAttack())
		{
			currentState = STATE_DEFEND;
		}
	}
	setGoal(mGoal);
}

void Squad::attack(TilePosition mGoal)
{
	if (mGoal.x() == -1 || mGoal.y() == -1) return;
	
	if (currentState != STATE_ATTACK)
	{
		if (!isUnderAttack())
		{
			if (isActive())
			{
				currentState = STATE_ATTACK;
			}
		}
	}

	if (isActive())
	{
		setGoal(mGoal);
	}
}

void Squad::assist(TilePosition mGoal)
{
	if (mGoal.x() == -1) return;

	if (currentState != STATE_ASSIST)
	{
		if (!isUnderAttack())
		{
			currentState = STATE_ASSIST;
			setGoal(mGoal);
		}
	}
}

void Squad::setGoal(TilePosition mGoal)
{
	if (isAttacking())
	{
		if (goal.x() != -1)
		{
			return;
		}
	}

	if (mGoal.x() != goal.x() || mGoal.y() != goal.y())
	{
		goalSetFrame = Broodwar->getFrameCount();
		if (isGround())
		{
			int d = (int)goal.getDistance(mGoal);
			if (d >= 10)
			{
				if ((int)agents.size() > 0)
				{
					Pathfinder::getInstance()->requestPath(getCenter(), mGoal);
					if (!Pathfinder::getInstance()->isReady(getCenter(), mGoal))
					{
						return;
					}
					path = Pathfinder::getInstance()->getPath(getCenter(), mGoal);
					
					arrivedFrame = -1;
					pathIndex = 20;
				}
			}
		}

		this->goal = mGoal;
		setMemberGoals(goal);
	}
}

TilePosition Squad::nextFollowMovePosition()
{
	if (path.size() <= 0)
	{
		return goal;
	}
	
	if (pathIndex >= (int)path.size())
	{
		return goal;
	}

	int cPathIndex = pathIndex - 20;
	if (cPathIndex < 0) cPathIndex = 0;
	TilePosition cGoal = path.at(cPathIndex);
	
	return cGoal;
}

TilePosition Squad::nextMovePosition()
{
	if (path.size() <= 0)
	{
		return goal;
	}
	if (isAir())
	{
		return goal;
	}

	if (pathIndex >= (int)path.size())
	{
		return goal;
	}

	if (arrivedFrame == -1)
	{
		for (int i = 0; i < (int)agents.size(); i++)
		{
			//Check if we have arrived at a checkpoint. For mixed squads,
			//air units does not count as having arrived.
			bool check = false;
			if (isGround() && !agents.at(i)->getUnitType().isFlyer()) check = true;
			if (isAir()) check = true;

			if (check)
			{
				int seekDist = agents.at(i)->getUnitType().sightRange() / 2;
				int dist = (int)agents.at(i)->getUnit()->getPosition().getDistance(Position(path.at(pathIndex)));
				if (dist <= seekDist)
				{
					arrivedFrame = Broodwar->getFrameCount();
					break;
				}
			}
		}
	}

	if (arrivedFrame != -1)
	{
		int cFrame = Broodwar->getFrameCount();
		if (cFrame - arrivedFrame >= 200) //100
		{
			pathIndex += 20; //20
			if (pathIndex >= (int)path.size())
			{
				pathIndex = (int)path.size() - 1;
			}
			arrivedFrame = -1;
		}
	}

	TilePosition cGoal = path.at(pathIndex);
	setMemberGoals(cGoal);

	return cGoal;
}

void Squad::clearGoal()
{
	this->goal = TilePosition(-1, -1);
	setMemberGoals(goal);
}

void Squad::setMemberGoals(TilePosition cGoal)
{
	if (isBunkerDefend()) return;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			agents.at(i)->setGoal(cGoal);
		}
	}
}

TilePosition Squad::getGoal()
{
	return goal;
}

bool Squad::hasGoal()
{
	int elapsed = Broodwar->getFrameCount() - goalSetFrame;
	if (elapsed >= 600)
	{
		if (!isAttacking())
		{
			goal = TilePosition(-1, -1);
		}
	}
	
	if (goal.x() < 0 || goal.y() < 0)
	{
		return false;
	}
	return true;
}

TilePosition Squad::getCenter()
{
	if (agents.size() == 1)
	{
		return agents.at(0)->getUnit()->getTilePosition();
	}

	int cX = 0;
	int cY = 0;
	int cnt = 0;

	//Calculate sum (x,y)
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			cX += agents.at(i)->getUnit()->getTilePosition().x();
			cY += agents.at(i)->getUnit()->getTilePosition().y();
			cnt++;
		}
	}

	//Calculate average (x,y)
	if(cnt > 0)
	{
		cX = cX / cnt;
		cY = cY / cnt;
	}

	//To make sure the center is in a walkable tile, we need to
	//find the unit closest to center
	TilePosition c = TilePosition(cX, cY);
	TilePosition bestSpot = c;
	double bestDist = 10000;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if ( (isAir() && agents.at(i)->getUnitType().isFlyer()) || (isGround() && !agents.at(i)->getUnitType().isFlyer()))
			{
				double dist = agents.at(i)->getUnit()->getTilePosition().getDistance(c);
				if (dist < bestDist)
				{
					bestDist = dist;
					bestSpot = agents.at(i)->getUnit()->getTilePosition();
				}
			}
		}
	}

	return bestSpot;
}

int Squad::getSize()
{
	int no = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive() && !agents.at(i)->getUnit()->isBeingConstructed())
		{
			no++;
		}
	}
	return no;
}

int Squad::getTotalUnits()
{
	int tot = 0;

	for (int i = 0; i < (int)setup.size(); i++)
	{
		tot += setup.at(i).no;
	}

	return tot;
}

int Squad::getStrength()
{
	int str = 0;

	for (int i = 1; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			str += agents.at(i)->getUnitType().destroyScore();
		}
	}

	return str;
}

bool Squad::isOffensive()
{
	if (type == OFFENSIVE) return true;
	if (type == SHUTTLE) return true;
	return false;
}

bool Squad::isDefensive()
{
	return type == DEFENSIVE;
}

bool Squad::isExplorer()
{
	return type == EXPLORER;
}

bool Squad::isSupport()
{
	return type == SUPPORT;
}

bool Squad::isBunkerDefend()
{
	return type == BUNKER;
}

bool Squad::isShuttle()
{
	return type == SHUTTLE;
}

bool Squad::isKite()
{
	return type == KITE;
}

bool Squad::isRush()
{
	return type == RUSH;
}

bool Squad::isGround()
{
	return moveType == GROUND;
}

bool Squad::isAir()
{
	return moveType == AIR;
}

bool Squad::hasUnits(UnitType type, int no)
{
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(type))
		{
			if (setup.at(i).current >= no)
			{
				//I have these units
				return true;
			}
		}
	}
	return false;
}

TilePosition Squad::getNextStartLocation()
{
	for(set<BaseLocation*>::const_iterator i=getStartLocations().begin();i!=getStartLocations().end();i++)
	{
		TilePosition basePos = (*i)->getTilePosition();
		if (!isVisible(basePos))
		{
			return basePos;
		}
		else
		{
			if ((int)agents.size() > 0)
			{
				UnitAgent* uagent = (UnitAgent*)agents.at(0);
				int eCnt = uagent->enemyUnitsWithinRange(10 * 32);
				if (eCnt > 0)
				{
					return TilePosition(-1, -1);
				}
			}

			hasVisited.push_back(basePos);
		}
	}
	return TilePosition(-1, -1);
}

bool Squad::isVisible(TilePosition pos)
{
	if (!ExplorationManager::canReach(Broodwar->self()->getStartLocation(), pos))
	{
		return true;
	}

	if (Broodwar->isVisible(pos))
	{
		return true;
	}

	if (getCenter().getDistance(pos) <= 3)
	{
		return true;
	}

	for (int i = 0; i < (int)hasVisited.size(); i++)
	{
		TilePosition vPos = hasVisited.at(i);
		if (vPos.x() == pos.x() && vPos.y() == pos.y())
		{
			return true;
		}
	}

	return false;
}
