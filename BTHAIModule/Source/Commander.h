#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include "Squad.h"
#include "BaseAgent.h"
#include "BuildplanEntry.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

struct SortSquadList {
	bool operator()(Squad*& sq1, Squad*& sq2)
	{
		if (sq1->getPriority() != sq2->getPriority())
		{
			return sq1->getPriority() < sq2->getPriority();
		}
		else
		{
			if (sq1->isRequired() && !sq2->isRequired()) return true;
			else return false;
		}
	}
};

/** The Commander class is the base class for commanders. The Commander classes are responsible for
 * which and when buildings to construct, when to do upgrades/techs, and which squads to build.
 * It is also responsible for finding defensive positions, launch attacks and where to launch an
 * attack.
 *
 * The Commander is implemented as a singleton class. Each class that needs to access Commander can
 * request an instance, and the correct commander (Terran/Protoss/Zerg) will be returned.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Commander {

private:
	bool chokePointFortified(TilePosition center);
	void sortSquadList();
	bool isOccupied(BWTA::Region* region);
	bool isEdgeChokepoint(Chokepoint* choke);
	TilePosition findDefensePos(Chokepoint* choke);
	double getChokepointPrio(TilePosition center);
	
	void checkNoSquadUnits();
	void assignUnit(BaseAgent* agent);

protected:
	vector<Squad*> squads;
	vector<BuildplanEntry> buildplan;
	int noWorkers;
	int noWorkersPerRefinery;
	static Commander* instance;
	int stage;

	int lastCallFrame;
	int currentState;
	bool debug_bp;
	bool debug_sq;
	static const int DEFEND = 0;
	static const int ATTACK = 1;

	Commander();

	/** Executes basic code for a commander. */
	void computeActionsBase();

	/** Checks the Commander buildplan, and add buildings,
	 * techs and upgrades to the planners. */
	void checkBuildplan();

	/** Stops the production of workers. */
	void cutWorkers();

public:
	/** Destructor. */
	~Commander();

	/** Returns the instance of the class. */
	static Commander* getInstance();

	/** Switch on/off buildplan debug info printing to screen. */
	void toggleBuildplanDebug();

	/** Switch on/off squads debug info printing to screen. */
	void toggleSquadsDebug();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the number of preferred workers, i.e. the
	 * number of workers should be built. */
	int getNoWorkers();

	/** Returns the preferred number of workers for a refinery. */
	int getWorkersPerRefinery();

	/** Used in debug modes to show goal of squads. */
	virtual void debug_showGoal();

	/** Checks if it is time to engage the enemy. This happens when all Required squads
	 * are active. */
	bool shallEngage();

	/** Updates the goals for all squads. */
	void updateGoals();

	/** Called each time a unit is created. The unit is then
	 * placed in a Squad. */
	void unitCreated(BaseAgent* agent);

	/** Called each time a unit is destroyed. The unit is then
	 * removed from its Squad. */
	void unitDestroyed(BaseAgent* agent);

	/* Checks if the specified unittype needs to be built. */
	bool needUnit(UnitType type);

	/** Removes a squad. */
	void removeSquad(int id);

	/** Returns the Squad with the specified id, or NULL if not found. */
	Squad* getSquad(int id);

	/** Returns all squads. */
	vector<Squad*> getSquads();

	/** Returns the position of the closest enemy building from the start position,
	 * or TilePosition(-1,-1) if not found. */
	TilePosition getClosestEnemyBuilding(TilePosition start);
	
	/** Checks if workers needs to attack. Happens if base is under attack and no offensive units
	 * are available. */
	bool checkWorkersAttack(BaseAgent* base);

	/** Tries to find a free squad to assist a building. */
	void assistBuilding(BaseAgent* building);

	/** Tries to find a free squad to assist a worker that is under attack. */
	void assistWorker(BaseAgent* worker);

	/** Checks if there are any removable obstacles nearby, i.e. minerals with less than 20 resources
	 * left. */
	void checkRemovableObstacles();

	/** Forces an attack, even if some squads are not full. */
	void forceAttack();

	/** Shows some info on the screen. */
	void printInfo();

	/** Searches for a chokepoint that is unfortified, i.e. does not contain for example a Bunker or defensive
	 * turret. Returns TilePosition(-1, -1) if no position was found. */
	TilePosition findUnfortifiedChokePoint();
	
	/** Searches for and returns a good chokepoint position to defend the territory. */
	TilePosition findChokePoint();

	/** Checks is a position is buildable. */
	bool isBuildable(TilePosition pos);

	/** Checks if there are any unfinished buildings that does not have an SCV working on them. Terran only. */
	bool checkUnfinishedBuildings();

	/** Assigns a worker to finish constructing an interrupted building. Terran only. */
	void finishBuild(BaseAgent* agent);

	/** The total killScore points of all own, destroyed units (not buildings). */
	int ownDeadScore;

	/** The total killScore points of all enemy, destroyed units (not buildings). */
	int enemyDeadScore;

	/** Adds a bunker squad when a Terran Bunker has been created. Returns
	 * the squadID of the bunker squad. */
	int addBunkerSquad();

	/** Removes the race from a string, Terran Marine = Marine. */
	static string format(string str);

};

#endif
