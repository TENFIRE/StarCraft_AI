#include "DropTankSquad.h"
#include "Commander.h"

void DropTankGroup::moveTankToShip(BaseAgent* tank)
{
	if (tank != NULL && dropShip != NULL)
	{
		if (tank->getUnit()->getGroundWeaponCooldown() > 0)
		{
			//move tank to dropship
		}
	}
}

void DropTankGroup::dropTank(BaseAgent* tank)
{
	if (tank != NULL && dropShip != NULL)
	{
		if (tank->getUnit()->getGroundWeaponCooldown() <= 0)
		{
			//drop tank
		}
	}
}

void DropTankGroup::computeActions(bool attack)
{
	Squad* squad = Commander::getInstance()->getSquad(dropShip->getSquadID());
	if (attack)
	{
		switch (state)
		{
		case DropTankGroup::BothGround:
			dropShip->setGoal(tank1->getUnit()->getTilePosition());
			moveTankToShip(tank1);
			moveTankToShip(tank2);
			break;
		case DropTankGroup::Tank1Ground:
			dropShip->setGoal(tank1->getUnit()->getTilePosition());
			dropTank(tank2);
			moveTankToShip(tank1);
			break;
		case DropTankGroup::Tank2Ground:
			dropShip->setGoal(tank2->getUnit()->getTilePosition());
			dropTank(tank1);
			moveTankToShip(tank2);
			break;
		case DropTankGroup::BothAir:
			/*
			if (In Range To Shoot)
			{
				dropShip->setGoal(dropShip->getUnit()->getTilePosition());
				dropTank(tank1);
				dropTank(tank2);
			}
			*/

			//modify goal to stop when the tanks is in range to shoot

			dropShip->setGoal(squad->getGoal());
			break;
		default:
			break;
		}
	}
	else
	{
		switch (state)
		{
		case DropTankGroup::BothGround:
			moveTankToShip(tank1);
			moveTankToShip(tank2);
			dropShip->setGoal(tank1->getUnit()->getTilePosition());
			break;
		case DropTankGroup::Tank1Ground:
			moveTankToShip(tank1);
			dropShip->setGoal(tank1->getUnit()->getTilePosition());
			break;
		case DropTankGroup::Tank2Ground:
			moveTankToShip(tank2);
			dropShip->setGoal(tank2->getUnit()->getTilePosition());
			break;
		case DropTankGroup::BothAir:
			dropShip->setGoal(squad->getGoal());
			break;
		default:
			break;
		}
	}
}


DropTankSquad::DropTankSquad(int mId, int mType, string mName, int mPriority)
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



DropTankSquad::DropTankSquad()
{
}

bool isTank(BaseAgent* agent)
{
	return agent->getUnitType() == UnitTypes::Terran_Siege_Tank_Tank_Mode || agent->getUnitType() == UnitTypes::Terran_Siege_Tank_Siege_Mode;
}

bool DropTankSquad::IsAgentActive(BaseAgent* agent)
{
	for (int i = 0; i < activeAgents.size(); i++)
		if (activeAgents[i] == agent)
			return false;
	return true;
}

void DropTankSquad::DisbandGroup(DropTankGroup* group)
{
	//remove all units in the group from acitveAgents and remove the group from groups
	
	for (vector<BaseAgent*>::iterator it = activeAgents.begin(); it != activeAgents.end();)
	{
		if ((*it) == group->dropShip || (*it) == group->tank1 || (*it) == group->tank2)
		{
			//delete *it;
			it = activeAgents.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (vector<DropTankGroup*>::iterator it = groups.begin(); it != groups.end();)
	{
		if ((*it) == group)
		{
			delete *it;
			it = groups.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void DropTankSquad::SetUpGroups()
{
	vector<BaseAgent*> freeTanks		= vector<BaseAgent*>();
	vector<BaseAgent*> freeDropShips	= vector<BaseAgent*>();
	vector<DropTankGroup*> halfGroups	= vector<DropTankGroup*>();

	//Dispand all groups that don't have a dropship
	//Alos put all the groups that is missing a tank in a list.
	for (int i = 0; i < groups.size(); i++)
	{
		if (!groups[i]->dropShip->isAlive())
			DisbandGroup(groups[i]);
		else
		{
			bool isFull = true;
			if (!groups[i]->tank1->isAlive())
			{
				groups[i]->tank1 = NULL;
				isFull = false;
			}
			if (!groups[i]->tank2->isAlive())
			{
				groups[i]->tank2 = NULL;
				isFull = false;
			}
			if (!isFull)
				halfGroups.push_back(groups[i]);
		}
	}

	//Gather all tanks and dropships that doesn't have a group yey in lists.
	for (int i = 0; i < agents.size(); i++)
	{
		if (isTank(agents[i]))
		{
			if (!IsAgentActive(agents[i]))
			{
				//If the agent is a tank and does not have a group yet. Add it to a list
				freeTanks.push_back(agents[i]);
			}
		}
		else if (agents[i]->getUnitType() == UnitTypes::Terran_Dropship)
		{
			if (!IsAgentActive(agents[i]))
			{
				//If the agent is a dropship and does not have a group yet. Add it to a list
				freeDropShips.push_back(agents[i]);
			}
		}
	}

	int j = 0;
	//fill up halfGroups
	for (int i = 0; i < halfGroups.size() && j < freeTanks.size(); i++)
	{
		if (j < freeTanks.size() && halfGroups[i]->tank1 == NULL)
		{
			halfGroups[i]->tank1 = freeTanks[j];
			j++;
		}
		if (j < freeTanks.size() && halfGroups[i]->tank2 == NULL)
		{
			halfGroups[i]->tank2 = freeTanks[j];
			j++;
		}
	}

	//Create new groups (accept halfgroups)
	for (int i = 0; i < freeDropShips.size() && j < freeTanks.size(); i++)
	{
		BaseAgent* dropship = freeDropShips[i];
		BaseAgent* tank1 = freeTanks[j];
		BaseAgent* tank2;
		j++;
		if (j < freeTanks.size())
			tank2 = freeTanks[j];
		else
			tank2 = NULL;

		DropTankGroup* newGroup = new DropTankGroup(dropship, tank1, tank2);
		groups.push_back(newGroup);
	}
}

void DropTankSquad::computeActions()
{
	Squad::computeActions();
	//modify goal, stop at goal - tank.range
	int	seconds = Broodwar->getFrameCount();

	if (currentState == STATE_DEFEND)
	{
		SetUpGroups();
	}

	for (int i = 0; i < groups.size(); i++)
	{
		//true if at goal and attacking
		groups[i]->computeActions(true);
	}

}
