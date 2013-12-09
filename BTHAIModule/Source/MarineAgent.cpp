#include "MarineAgent.h"
#include "NavigationAgent.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"
#include <cmath>

MarineAgent::MarineAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MarineAgent";
	
	goal = TilePosition(-1, -1);
	realGoal = TilePosition(-1, -1);
	isProMicroing = false;
}

void MarineAgent::computeActions()
{
	/*
	Unit* closestUnit = getClosestOrganicEnemy(this->getGroundRange());
	if (closestUnit != NULL &&
		this->unit->getGroundWeaponCooldown() > 0 &&
		Position(closestUnit->getPosition().x() - this->getUnit()->getPosition().x(), closestUnit->getPosition().y() - this->getUnit()->getPosition().y()).getLength() < this->getGroundRange() )
	{
		this->temporaryGoal = computeProMicroGoal(closestUnit);

		if (this->temporaryGoal != TilePosition(-1, -1))
		{
			Broodwar->printf("TilePos: %i, %i", temporaryGoal.x(), temporaryGoal.y());
			if (!isProMicroing)
				this->realGoal = goal;
			setGoal(temporaryGoal);
			isProMicroing = true;
		}
	}
	else if (this->unit->getGroundWeaponCooldown() == 0)
	{
		if (isProMicroing)
		{
			setGoal(realGoal);
			isProMicroing = false;
		}
	}

	NavigationAgent::getInstance()->computeMove(this, goal, isProMicroing);	*/

	if (this->unit->getStimTimer() == 0 && this->unit->isAttacking())
	{
		this->unit->useTech(TechTypes::Stim_Packs);
	}
	if (this->unit->getTarget() != NULL && this->unit->getTarget()->isCloaked() && !this->unit->getTarget()->isVisible())
	{
		this->doScannerSweep(this->unit->getTarget()->getTilePosition());
	}

	NavigationAgent::getInstance()->computeMove(this, goal, false);
}

TilePosition MarineAgent::computeProMicroGoal(Unit* closestUnit)
{
	float distanceToMaxRange = this->getGroundRange() - Position(closestUnit->getPosition().x() - this->getUnit()->getPosition().x(), closestUnit->getPosition().y() - this->getUnit()->getPosition().y()).getLength();
	float angle = atan2f(this->unit->getPosition().y() - closestUnit->getPosition().y(), this->unit->getPosition().x() - closestUnit->getPosition().x());
	Position newPosition = Position(this->unit->getPosition().x() + (cosf(angle) * distanceToMaxRange), this->unit->getPosition().y() + (sinf(angle) * distanceToMaxRange));
	if (TilePosition(newPosition).isValid())
	{
		return TilePosition(newPosition);
	}
	return TilePosition(-1, -1);
}
