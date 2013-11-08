#ifndef __VALKYRIEAGENT_H__
#define __VALKYRIEAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The ValkyrieAgent handles Terran Valkyrie flying units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ValkyrieAgent : public UnitAgent {

private:

public:
	ValkyrieAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
