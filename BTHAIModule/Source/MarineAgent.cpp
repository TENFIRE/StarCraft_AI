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
	Unit* closestUnit = getClosestOrganicEnemy(this->getGroundRange());
	Squad* squad = Commander::getInstance()->getSquad(this->squadID);
	if (closestUnit != NULL &&
		this->unit->getGroundWeaponCooldown() > 0 &&
		Position(closestUnit->getPosition().x() - this->getUnit()->getPosition().x(), closestUnit->getPosition().y() - this->getUnit()->getPosition().y()).getLength() < this->getGroundRange())
	{
		this->temporaryGoal = computeProMicroGoal(closestUnit);

		if (this->temporaryGoal != TilePosition(-1, -1))
		{
			//Broodwar->printf("TilePos: %i, %i", temporaryGoal.x(), temporaryGoal.y());
			Broodwar->setLocalSpeed(80);
			setGoal(temporaryGoal);
			isProMicroing = true;
		}
	}
	else if (this->unit->getGroundWeaponCooldown() <= 0)
	{
		if (isProMicroing)
		{
			//if (closestUnit != NULL)
			//	this->unit->attack(closestUnit);
			//setGoal(squad->getGoal());
			isProMicroing = false;
		}
	}
	
	NavigationAgent::getInstance()->computeMove(this, goal, isProMicroing);	

	//Commander::getInstance()->getSquad(this->squadID);
	//NavigationAgent::getInstance()->computeMove(this, goal, false);
}

TilePosition MarineAgent::computeProMicroGoal(Unit* closestUnit)
{
	float distance = Position(this->unit->getPosition() - closestUnit->getPosition()).getLength();
	float angle = atan2((float)this->unit->getPosition().y() - closestUnit->getPosition().y(), (float)this->unit->getPosition().x() - closestUnit->getPosition().x());
	Position position = Position(this->unit->getPosition().x() + (this->unit->getPosition().x() - closestUnit->getPosition().x()) + cosf(angle) * (this->getGroundRange() - distance), this->unit->getPosition().y() + (this->unit->getPosition().y() - closestUnit->getPosition().y()) + sinf(angle) * (this->getGroundRange() - distance));
	TilePosition tilePosition = TilePosition(position);
	/*Broodwar->printf("Distance: %f, Angle: %f", distance, angle);
	Broodwar->printf("Old Pos: %i, %i... New Pos: %i, %i", this->unit->getPosition().x(), this->unit->getPosition().y(), position.x(), position.y());
	Broodwar->setLocalSpeed(80);*/
	if (tilePosition.isValid())
		return tilePosition;
	else
		return TilePosition(-1, -1);;
}
