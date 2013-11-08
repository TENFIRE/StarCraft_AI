#ifndef __BATTLECRUISERAGENT_H__
#define __BATTLECRUISERAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The BattlecruiserAgent handles Terran Battlecruiser flying units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BattlecruiserAgent : public UnitAgent {

private:
	
public:
	BattlecruiserAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
