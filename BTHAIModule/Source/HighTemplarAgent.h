#ifndef __HIGHTEMPLARAGENT_H__
#define __HIGHTEMPLARAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The HighTemplarAgent handles Protoss High Templar units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class HighTemplarAgent : public UnitAgent {

private:
	
public:
	HighTemplarAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
