#ifndef __GHOSTAGENT_H__
#define __GHOSTAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The GhostAgent handles Terran Ghost units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class GhostAgent : public UnitAgent {

private:
	
public:
	GhostAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
