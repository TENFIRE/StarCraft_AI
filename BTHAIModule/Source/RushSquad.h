#ifndef __RUSHSQUAD_H__
#define __RUSHSQUAD_H__

#include "Squad.h"
using namespace BWAPI;
using namespace std;

class RushSquad : public Squad
{
private:
	bool	isBunkerMode;
	int		bunkeredUnits;
	Unit*	bunkerUnit;

public:
	/** Creates a squad with a unique id, a type (Offensive, Defensive, Exploration, Support),
	 * a name (for example AirAttackSquad, MainGroundSquad).
	 * Higher priority squads gets filled before lower prio squads. Lower prio value is considered
	 * higher priority. A squad with priority of 1000 or more will not be built. This can be used
	 * to create one-time squads that are only filled once.
	 */
	RushSquad( int mId, int mType, string mName, int mPriority );
	RushSquad();

	/** Called each update to issue orders. */
	virtual void computeActions();


	void	setBunkerMode( bool Value );
}


#endif