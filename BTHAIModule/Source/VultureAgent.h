#ifndef __VULTUREAGENT_H__
#define __VULTUREAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The VultureAgent handles Terran Vulture units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class VultureAgent : public UnitAgent {

private:
	
public:
	VultureAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
