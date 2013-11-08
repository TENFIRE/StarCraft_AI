#ifndef __ZERGSTRATEGY_H__
#define __ZERGSTRATEGY_H__

#include "Commander.h"
#include "Squad.h"
#include "BaseAgent.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

/** 
 * Implement your Zerg strategy here
 */
class ZergStrategy : public Commander {

private:
	Squad* mainSquad;

public:
	ZergStrategy();

	/** Destructor. */
	~ZergStrategy();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the unique id for this strategy. */
	static string getStrategyId()
	{
		return "ZergStrategy";
	}
};

#endif
