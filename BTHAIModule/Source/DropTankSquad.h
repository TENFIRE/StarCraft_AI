#ifndef __DROPTANKSQUAD_H__
#define __DROPTANKSQUAD_H__

#include "Squad.h"
using namespace BWAPI;
using namespace std;


class DropTankGroup
{
	enum State
	{
		BothGround, Tank1Ground, Tank2Ground, BothAir
	};
	State state;

	void moveTankToShip(BaseAgent* tank);
	void dropTank(BaseAgent* tank);

public:
	BaseAgent* dropShip;
	BaseAgent* tank1;
	BaseAgent* tank2;

	
	DropTankGroup(BaseAgent* ds, BaseAgent* t1, BaseAgent* t2)
	{
		state = BothGround;
		dropShip = ds;
		tank1 = t1;
		tank2 = t2;
	}

	
	void computeActions(bool attack);
};

class DropTankSquad : public Squad
{
	vector<DropTankGroup*> groups;
	vector<BaseAgent*> activeAgents;

	void DisbandGroup(DropTankGroup* group);
	bool IsAgentActive(BaseAgent* agent);
	void SetUpGroups();

public:
	DropTankSquad(int mId, int mType, string mName, int mPriority);
	DropTankSquad();

	/** Called each update to issue orders. */
	virtual void computeActions();
};

#endif