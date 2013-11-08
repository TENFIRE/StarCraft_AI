#ifndef __WALKTILE_H__
#define __WALKTILE_H__

#include <BWAPI.h>
using namespace BWAPI;
using namespace std;

/** This class represents a Starcraft walk tile. Walk tiles are 8 positions/0.25 terrain tile.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class WalkTile {

private:
	int x;
	int y;

public:
	/** Constructs a walk tile from a terrain tile. */
	WalkTile(TilePosition tp);

	/** Constructs a walk tile from a position. */
	WalkTile(Position pos);

	/** Constructs a walk tile for a unit. */
	WalkTile(Unit* unit);

	/** Constructs a walk tile. */
	WalkTile(int x, int y);

	/** X position. */
	int X();

	/** Y position. */
	int Y();

	/** Converts this walk tile to a terrain tile. */
	TilePosition getTilePosition();
	
	/** Converts this walk tile to a position. */
	Position getPosition();

	/** Returns the distance (in walk tiles) between two walk tiles. */
	double getDistance(WalkTile wt);
};

#endif
