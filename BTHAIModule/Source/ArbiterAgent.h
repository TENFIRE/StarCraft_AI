#ifndef __ARBITERAGENT_H__
#define __ARBITERAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The ArbiterAgent handles Protoss Arbiter flying units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ArbiterAgent : public UnitAgent {

private:

public:
	ArbiterAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
