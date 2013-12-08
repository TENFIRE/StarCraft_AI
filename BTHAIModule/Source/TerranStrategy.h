#ifndef __TERRANSTRATEGY_H__
#define __TERRANSTRATEGY_H__

#include "Commander.h"
#include "Squad.h"
#include "BaseAgent.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

/**  
 * Implement your Terran strategy here.
 */
 struct LocationData
 {
	LocationData()
	{
		Location		=	Position(-1, -1);
		Attractiveness	=	0;
	}
	Position	Location;
	int			Attractiveness;
 };

class TerranStrategy : public Commander {
private:
	Squad* mainSquad;
	vector<Position>		positionsToScan;
	vector<LocationData>	enemyLocationsFromScans;

	void		addEnemyLocation( Position P, int Factor );
	void		addPossibleScanLocations( void );
	Position	getNextScanPosition( void );

public:
	TerranStrategy();

	/** Destructor. */
	~TerranStrategy();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the unique id for this strategy. */
	static string getStrategyId()
	{
		return "TerranStrategy";
	}

	static	vector<Unit*>	CommandCenters;
	static	vector<Unit*>	ComsatStations;
};

#endif
