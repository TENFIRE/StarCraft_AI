#ifndef __DEFENSELOCATOR_H__
#define __DEFENSELOCATOR_H__

#include "BaseAgent.h"
#include <vector>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

struct DefensePos
{
	string maphash;
	TilePosition homeBase;
	TilePosition defendBase;
	TilePosition pos;

	DefensePos(int hx, int hy, int dx, int dy, int px, int py, string mMaphash)
	{
		homeBase = TilePosition(hx,hy);
		defendBase = TilePosition(dx,dy);
		pos = TilePosition(px,py);
		maphash = mMaphash;
	}

	bool matches(TilePosition mHomeBase, TilePosition mDefendBase)
	{
		double d1 = homeBase.getDistance(mHomeBase);
		if (d1 <= 4)
		{
			double d2 = defendBase.getDistance(mDefendBase);
			if (d2 <= 4)
			{
				return true;
			}
		}
		return false;
	}
};

/** This class holds manually selected defense positions for a map. The Commander
 * first checks if a defense position has been added to the DefenseLocator, otherwise
 * the standard choke point defense strategy is used.
 *
 * Note: The functionality is heavily dependent on that defense positions are choosen
 * wisely.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class DefenseLocator {

private:
	DefenseLocator();
	static DefenseLocator* instance;
	vector<DefensePos> data;
	
public:
	/** Destructor */
	~DefenseLocator();

	/** Returns the instance of the class. */
	static DefenseLocator* getInstance();

	/** Returns the best defense position for the last constructed
	 * base on the specified map.*/
	TilePosition getBaseDefensePos(string mMaphash);

};

#endif
