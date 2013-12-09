#ifndef __MARINEAGENT_H__
#define __MARINEAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The MarineAgent handles Terran Marine units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MarineAgent : public UnitAgent {

private:
	TilePosition realGoal;
	TilePosition temporaryGoal;
	bool isProMicroing;
	
	TilePosition computeProMicroGoal(Unit* closestUnit);
public:
	MarineAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
