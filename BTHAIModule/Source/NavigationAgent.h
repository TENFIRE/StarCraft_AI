#ifndef __NAVIGATIONAGENT_H__
#define __NAVIGATIONAGENT_H__

#include "BaseAgent.h"
#include "WalkTile.h"
using namespace BWAPI;
using namespace std;

/** The bot uses two techniques for navigation: if no enemy units are close units navigate using the built in pathfinder in
 * Starcraft. If enemy units are close, own units uses potential fields to engage and surround the enemy.
 * The NavigationAgent class is the main class for the navigation system, and it switches between built in pathfinder and
 * potential fields when necessary.
 *
 * The NavigationAgent is implemented as a singleton class. Each class that needs to access NavigationAgent can request an instance,
 * and all classes shares the same NavigationAgent instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class NavigationAgent {

private:
	NavigationAgent();
	static NavigationAgent* instance;
	static bool instanceFlag;

	/** Moves a unit to the specified goal using the pathfinder, and stops at a distance where the
	* potential field navigation system should be used instead. */
	bool moveToGoal(BaseAgent* agent, TilePosition checkpoint, TilePosition goal);
	
	/** Calculates the potential field values for an attacking unit. */
	float getAttackingUnitP(BaseAgent* agent, WalkTile wp, bool defensive);
	float getDefendingUnitP(BaseAgent* agent, WalkTile wp);
	
	int checkRange;
	int mapW;
	int mapH;

	Color getColor(float p);

public:
	/** Destructor */
	~NavigationAgent();

	/** Returns the instance to the class. */
	static NavigationAgent* getInstance();

	/** Is used to compute and execute movement commands for attacking units using the potential field
	 * navigation system. */
	void computeMove(BaseAgent* agent, TilePosition goal, bool defensive);

	/** Computes a pathfinding move (no enemy units in range) */
	void computePathfindingMove(BaseAgent* agent, TilePosition goal);
	
	/** Computes a PF move (enemy units within range) */
	void computePotentialFieldMove(BaseAgent* agent, bool defensive);
	void computePotentialFieldDefendingMove(BaseAgent* agent);

	/** Displays a debug view of the potential fields for an agent. */
	void displayPF(BaseAgent* agent);
};

#endif
