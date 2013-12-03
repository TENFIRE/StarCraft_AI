#ifndef __MARINE_SQUAD_H__
#define __MARINE_SQUAD_H__

#include "Squad.h"
using namespace BWAPI;
using namespace std;

/** The Squad class represents a squad of units with a shared goal, for example
 * attacking the enemy or defending the base. The Squad can be built up from
 * different combinations and numbers of UnitTypes. 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MarineSquad : public Squad
{
protected:
	int		type_default;
	float	stimpacks_timer;
public:
	/** Default constructor. */
	MarineSquad();
	
	/** Creates a squad with a unique id, a type (Offensive, Defensive, Exploration, Support),
	 * a name (for example AirAttackSquad, MainGroundSquad).
	 * Higher priority squads gets filled before lower prio squads. Lower prio value is considered
	 * higher priority. A squad with priority of 1000 or more will not be built. This can be used
	 * to create one-time squads that are only filled once.
	 */
	MarineSquad(int mId, int mType, string mName, int mPriority);

	void	computeActions();
};

#endif
