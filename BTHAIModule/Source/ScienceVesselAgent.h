#ifndef __SCIENCEVESSELAGENT_H__
#define __SCIENCEVESSELAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The ScienceVesselAgent handles Terran Science Vessels units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ScienceVesselAgent : public UnitAgent {

private:
	
public:
	ScienceVesselAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
