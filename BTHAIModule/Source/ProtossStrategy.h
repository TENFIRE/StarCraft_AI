#ifndef __PROTOSSSTRATEGY_H__
#define __PROTOSSSTRATEGY_H__

#include "Commander.h"
#include "Squad.h"
#include "BaseAgent.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

/**  
 * Implement your Protoss strategy here.
 */
class ProtossStrategy : public Commander {

private:
	Squad* mainSquad;
	
public:
	ProtossStrategy();

	/** Destructor. */
	~ProtossStrategy();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the unique id for this strategy. */
	static string getStrategyId()
	{
		return "ProtossStrategy";
	}
};

#endif
