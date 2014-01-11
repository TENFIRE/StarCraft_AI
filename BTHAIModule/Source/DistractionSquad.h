#ifndef __DISTRACTIONSQUAD_H__
#define __DISTRACTIONSQUAD_H__

#include "Squad.h"
;using namespace BWAPI;
using namespace std;

class DistractionSquad : public Squad
{
private:

public:
	/** Creates a squad with a unique id, a type (Offensive, Defensive, Exploration, Support),
	* a name (for example AirAttackSquad, MainGroundSquad).
	* Higher priority squads gets filled before lower prio squads. Lower prio value is considered
	* higher priority. A squad with priority of 1000 or more will not be built. This can be used
	* to create one-time squads that are only filled once.
	*/
	DistractionSquad(int mId, int mType, string mName, int mPriority);
	DistractionSquad();

	/** Called each update to issue orders. */
	virtual void computeActions();
}


#endif