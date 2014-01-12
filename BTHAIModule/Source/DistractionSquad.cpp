#include "DistractionSquad.h"
#include "AgentManager.h"

DistractionSquad::DistractionSquad()
{
}

DistractionSquad::DistractionSquad(int mId, int mType, string mName, int mPriority)
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

void DistractionSquad::computeActions()
{
	Squad::computeActions();
	int	seconds = Broodwar->getFrameCount();	//	Only if fastest


}