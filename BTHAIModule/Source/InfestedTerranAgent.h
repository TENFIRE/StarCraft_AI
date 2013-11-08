#ifndef __INFESTEDTERRANAGENT_H__
#define __INFESTEDTERRANAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The InfestedTerranAgent handles Zerg Infested Terran units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class InfestedTerranAgent : public UnitAgent {

private:

public:
	InfestedTerranAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
