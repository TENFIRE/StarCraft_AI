#ifndef __DARKTEMPLARAGENT_H__
#define __DARKTEMPLARAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The DarkTemplarAgent handles Protoss Dark Templar units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class DarkTemplarAgent : public UnitAgent {

private:

public:
	DarkTemplarAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
