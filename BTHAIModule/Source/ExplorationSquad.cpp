#include "ExplorationSquad.h"
#include "UnitAgent.h"
#include "AgentManager.h"
#include "ExplorationManager.h"

ExplorationSquad::ExplorationSquad(int mId, string mName, int mPriority)
{
	this->id = mId;
	this->type = EXPLORER;
	this->moveType = AIR;
	this->name = mName;
	this->priority = mPriority;
	activePriority = priority;
	active = false;
	required = false;
	goal = Broodwar->self()->getStartLocation();
	goalSetFrame = 0;
	currentState = STATE_NOT_SET;
}

bool ExplorationSquad::isActive()
{
	return active;
}

void ExplorationSquad::defend(TilePosition mGoal)
{

}

void ExplorationSquad::attack(TilePosition mGoal)
{

}

void ExplorationSquad::assist(TilePosition mGoal)
{

}

void ExplorationSquad::computeActions()
{
	if (!active)
	{
		//Check if we need workers in the squad
		for (int i = 0; i < (int)setup.size(); i++)
		{
			if (setup.at(i).current < setup.at(i).no && setup.at(i).type.isWorker())
			{
				int no =  setup.at(i).no - setup.at(i).current;
				for (int j = 0; j < no; j++)
				{
					BaseAgent* w = AgentManager::getInstance()->findClosestFreeWorker(Broodwar->self()->getStartLocation());
					if (w != NULL) addMember(w);
				}
			}
		}

		if (isFull())
		{
			active = true;
		}
		return;
	}

	//First, remove dead agents
	for(int i = 0; i < (int)agents.size(); i++)
	{
		if(!agents.at(i)->isAlive())
		{
			agents.erase(agents.begin() + i);
			i--;
		}
	}

	//All units dead, go back to inactive
	if ((int)agents.size() == 0)
	{
		active = false;
		return;
	}

	if (active)
	{
		if (activePriority != priority)
		{
			priority = activePriority;
		}

		TilePosition nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
		if (nGoal.x() >= 0)
		{
			this->goal = nGoal;
			setMemberGoals(goal);
		}
	}
}

void ExplorationSquad::clearGoal()
{
	
}

TilePosition ExplorationSquad::getGoal()
{
	return goal;
}

bool ExplorationSquad::hasGoal()
{
	if (goal.x() < 0 || goal.y() < 0)
	{
		return false;
	}
	return true;
}
