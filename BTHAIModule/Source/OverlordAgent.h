#ifndef __OVERLORDAGENT_H__
#define __OVERLORDAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The OverlordAgent handles Zerg Overlord units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class OverlordAgent : public UnitAgent {

private:
	
public:
	OverlordAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
