#ifndef __MUTALISKAGENT_H__
#define __MUTALISKAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The MutaliskAgent handles Zerg Mutalisk units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MutaliskAgent : public UnitAgent {

private:
	
public:
	MutaliskAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
