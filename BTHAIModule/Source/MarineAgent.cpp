#include "MarineAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"
#include <math.h>

MarineAgent::MarineAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MarineAgent";
	isProMicroing = false;

	goal = TilePosition(-1, -1);
}

void MarineAgent::computeActions()
{
	//	Stim pack logic
	if( unit->isAttacking() && unit->getStimTimer() <= 0 )
		unit->useTech( TechTypes::Stim_Packs );

	// Cloaked unit scanning logic
	if (unit->getTarget() != NULL && unit->getTarget()->isCloaked() && !unit->getTarget()->isVisible())
	{
		Broodwar->printf(unit->getTarget()->getType().c_str());
		this->doScannerSweep(unit->getTarget()->getTilePosition());
	}
	
	// Pro micro logic
	Squad* squad = Commander::getInstance()->getSquad(this->squadID);
	
	bool defensive = false;
	// if the squad is attacking we check if we need to adjust the goal of the unit 	
	if (squad->isAttacking())
	{
		/*
			If the unit can shoot (no cooldown) we try to attack the 
			closest target within the (groundrange * 1.5).
			If we don't find an enemy we set the goal to the squadgoal
		*/
		if (unit->getGroundWeaponCooldown() <= 0/* && isProMicroing*/)
		{
			Unit* closestUnit = getClosestOrganicEnemy(this->getGroundRange() * 1.5f);

			if (closestUnit != NULL)
			{
				//Broodwar->printf("attack closest");
				this->unit->attack(closestUnit);
			}
			else
			{
				//Broodwar->printf("squad goal!");
				setGoal(squad->getGoal());
			}
		}
		/*
			If the unit can't shoot we try to move away from the closest target, so we stand groundrange distance from them.
		*/
		else
		{
			Unit* closestUnit = getClosestOrganicEnemy(this->getGroundRange());

			if (closestUnit != NULL)
			{
				double length = (closestUnit->getPosition() - unit->getPosition()).getLength();

				if (length < getGroundRange())
				{
					this->temporaryGoal = computeProMicroGoal(closestUnit);

					if (this->temporaryGoal != TilePosition(-1, -1))
					{
						//Broodwar->printf("TilePos: %i, %i", temporaryGoal.x(), temporaryGoal.y());
						//Broodwar->setLocalSpeed(80);
						setGoal(temporaryGoal);
						//Broodwar->printf("mirco!");
					}
				//	else
					//	Broodwar->printf("fail tilePos");
				}
			//	else
			//		Broodwar->printf("im already fine!");
			}
			//else
				Broodwar->printf("no unit");

			defensive = true;
		}
	}

	
	NavigationAgent::getInstance()->computeMove(this, goal, defensive);

	//Commander::getInstance()->getSquad(this->squadID);
	//NavigationAgent::getInstance()->computeMove(this, goal, false);
}

TilePosition MarineAgent::computeProMicroGoal(Unit* closestUnit)
{
	/*
		We are calculating a vector pointing from the target to this unit.
		Then we adjust the length of the vector to the groundrange of the unit.
		At last we calculate the goal by adding the vector to the current position.	
	*/
	Position tempP = this->unit->getPosition() - closestUnit->getPosition();
	float distance = tempP.getLength();
	float temp = getGroundRange() / distance;

	tempP = Position(tempP.x() * temp, tempP.y() * temp);
	tempP += unit->getPosition();

	TilePosition tilePosition = TilePosition(tempP);
	/*Broodwar->printf("Distance: %f, Angle: %f", distance, angle);
	Broodwar->printf("Old Pos: %i, %i... New Pos: %i, %i", this->unit->getPosition().x(), this->unit->getPosition().y(), position.x(), position.y());
	Broodwar->setLocalSpeed(80);*/
	if (tilePosition.isValid())
		return tilePosition;
	else
		return TilePosition(-1, -1);
}
