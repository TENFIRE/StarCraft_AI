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
	/*
	if (isAttacking())
	{

		//attack
		if (unit->getGroundWeaponCooldown() <= 0 && isProMicroing)
		{
			Unit* closestUnit = getClosestOrganicEnemy(this->getGroundRange());

			if (closestUnit != NULL)
			{
				this->unit->attack(closestUnit);
			}
			else
			{
				setGoal(squad->getGoal());
			}
			Broodwar->printf("attack!");
			NavigationAgent::getInstance()->computeMove(this, goal, false);
			isProMicroing = false;

		}
		//micro
		else if (!isProMicroing)
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
						Broodwar->setLocalSpeed(80);
						setGoal(temporaryGoal);
						Broodwar->printf("mirco!");
						NavigationAgent::getInstance()->computeMove(this, goal, true);
						isProMicroing = true;
					}
					else
						Broodwar->printf("fail tilePos");
				}
				else
					Broodwar->printf("im allready fine!");
			}
			else
				Broodwar->printf("no unit");
		}
	}
	*/

	//Commander::getInstance()->getSquad(this->squadID);
	//NavigationAgent::getInstance()->computeMove(this, goal, false);
}

TilePosition MarineAgent::computeProMicroGoal(Unit* closestUnit)
{
	

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
