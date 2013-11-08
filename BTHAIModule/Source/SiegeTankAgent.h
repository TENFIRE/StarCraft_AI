#ifndef __SIEGETANKAGENT_H__
#define __SIEGETANKAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The SiegeTankAgent handles Terran Siege Tank units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class SiegeTankAgent : public UnitAgent {
	
public:
	SiegeTankAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
