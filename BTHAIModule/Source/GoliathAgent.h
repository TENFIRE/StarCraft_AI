#ifndef __GOLIATHAGENT_H__
#define __GOLIATHAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The GoliathAgent handles Terran Goliath units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class GoliathAgent : public UnitAgent {

private:

public:
	GoliathAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
