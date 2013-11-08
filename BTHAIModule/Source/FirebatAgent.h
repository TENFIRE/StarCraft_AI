#ifndef __FIREBATAGENT_H__
#define __FIREBATAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The FirebatAgent handles Terran Firebat units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class FirebatAgent : public UnitAgent {

private:

public:
	FirebatAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
