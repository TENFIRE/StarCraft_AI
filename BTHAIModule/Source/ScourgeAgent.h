#ifndef __SCOURGEAGENT_H__
#define __SCOURGEAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The ScourgeAgent handles Zerg Scourge units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ScourgeAgent : public UnitAgent {

private:

public:
	ScourgeAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
