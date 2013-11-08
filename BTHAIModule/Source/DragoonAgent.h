#ifndef __DRAGOONAGENT_H__
#define __DRAGOONAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The DragoonAgent handles Protoss Dragoon units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class DragoonAgent : public UnitAgent {

private:
	
public:
	DragoonAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
