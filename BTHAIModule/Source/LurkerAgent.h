#ifndef __LURKERAGENT_H__
#define __LURKERAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The LurkerAgent handles Zerg Lurker units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class LurkerAgent : public UnitAgent {

private:

public:
	LurkerAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
