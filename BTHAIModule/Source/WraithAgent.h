#ifndef __WRAITHAGENT_H__
#define __WRAITHAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The WraithAgent handles Terran Wraith flying units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class WraithAgent : public UnitAgent {

private:
	
public:
	WraithAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
