#include "BuildingPlacer.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "Constructor.h"
#include "Commander.h"
#include "Profiler.h"

BuildingPlacer* BuildingPlacer::instance = NULL;

BuildingPlacer::BuildingPlacer()
{
	w = Broodwar->mapWidth();
	h = Broodwar->mapHeight();
	range = 40;

	Unit* worker = findWorker(Broodwar->self()->getStartLocation());

	cover_map = new int*[w];
	for(int i = 0 ; i < w ; i++)
	{
		cover_map[i] = new int[h];

		//Fill from static map and Region connectability
		for (int j = 0; j < h; j++)
		{
			int ok = BUILDABLE;
			if (!Broodwar->isBuildable(i, j))
			{
				ok = BLOCKED;
			}

			cover_map[i][j] = ok;
		}
	}

	//Fill from current agents
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isBuilding())
		{
			Corners c = getCorners(agent->getUnit());
			fill(c);
		}
	}

	//Fill from minerals
	for(set<Unit*>::iterator m = Broodwar->getMinerals().begin(); m != Broodwar->getMinerals().end(); m++)
	{
		Corners c;
		c.x1 = (*m)->getTilePosition().x() - 2;
		c.y1 = (*m)->getTilePosition().y() - 2;
		c.x2 = (*m)->getTilePosition().x() + 2;
		c.y2 = (*m)->getTilePosition().y() + 2;
		fill(c);

		cover_map[c.x1+2][c.y1+2] = MINERAL;
	}

	//Fill from gas
	for(set<Unit*>::iterator m = Broodwar->getGeysers().begin(); m != Broodwar->getGeysers().end(); m++)
	{
		Corners c;
		c.x1 = (*m)->getTilePosition().x() - 2;
		c.y1 = (*m)->getTilePosition().y() - 2;
		c.x2 = (*m)->getTilePosition().x() + 5;
		c.y2 = (*m)->getTilePosition().y() + 3;
		fill(c);

		cover_map[c.x1+2][c.y1+2] = GAS;
	}

	//Fill from narrow chokepoints
	if (analyzed)
	{
		for(set<BWTA::Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
		{
			for(set<Chokepoint*>::const_iterator c=(*i)->getChokepoints().begin();c!=(*i)->getChokepoints().end();c++)
			{
				if ((*c)->getWidth() <= 4 * 32)
				{
					TilePosition center = TilePosition((*c)->getCenter());
					Corners c;
					c.x1 = center.x() - 1;
					c.x2 = center.x() + 1;
					c.y1 = center.y() - 1;
					c.y2 = center.y() + 1;
					fill(c);
				}
			}
		}
	}

	mapData = MapDataReader();
	mapData.readMap();
}

BuildingPlacer::~BuildingPlacer()
{
	for(int i = 0 ; i < w ; i++)
	{
		delete[] cover_map[i];
	}
	delete[] cover_map;

	instance = NULL;
}

BuildingPlacer* BuildingPlacer::getInstance()
{
	if (instance == NULL)
	{
		instance = new BuildingPlacer();
	}
	return instance;
}


Unit* BuildingPlacer::findWorker(TilePosition spot)
{
	BaseAgent* worker = AgentManager::getInstance()->findClosestFreeWorker(spot);
	if (worker != NULL)
	{
		return worker->getUnit();
	}
	return NULL;
}

bool BuildingPlacer::positionFree(TilePosition pos)
{
	if (cover_map[pos.x()][pos.y()] == BUILDABLE)
	{
		return true;
	}
	return false;
}

void BuildingPlacer::blockPosition(TilePosition buildSpot)
{
	if (buildSpot.x() == -1)
	{
		//Error check
		return;
	}
	cover_map[buildSpot.x()][buildSpot.y()] = BLOCKED;
}

bool BuildingPlacer::canBuild(UnitType toBuild, TilePosition buildSpot)
{
	Corners c = getCorners(toBuild, buildSpot);

	//Step 1: Check BuildingPlacer.
	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] != BUILDABLE)
				{
					//Cant build here.
					return false;
				}
			}
			else
			{
				//Out of bounds
				return false;
			}
		}
	}

	//Step 2: Check if path is available
	if (!ExplorationManager::canReach(Broodwar->self()->getStartLocation(), buildSpot))
	{
		return false;
	}

	//Step 3: Check canBuild
	Unit* worker = findWorker(buildSpot);
	if (worker == NULL)
	{
		//No worker available
		return false;
	}

	//Step 4: Check any units on tile
	/*if (AgentManager::getInstance()->unitsInArea(buildSpot, toBuild.tileWidth(), toBuild.tileHeight(), worker->getID()))
	{
		return false;
	}*/

	//Step 5: If Protoss, check PSI coverage
	if (Constructor::isProtoss())
	{
		if (toBuild.requiresPsi())
		{
			if (!Broodwar->hasPower(buildSpot, toBuild))
			{
				return false;
			}
		}

		//Spread out Pylons
		if (toBuild.getID() == UnitTypes::Protoss_Pylon.getID())
		{
			if (Broodwar->hasPower(buildSpot))
			{
				return false;
			}
		}
	}

	//Step 6: If Zerg, check creep
	if (Constructor::isZerg())
	{
		if (toBuild.requiresCreep())
		{
			for (int x = buildSpot.x(); x < buildSpot.x() + toBuild.tileWidth(); x++)
			{
				for (int y = buildSpot.y(); y < buildSpot.y() + toBuild.tileHeight(); y++)
				{
					if (!Broodwar->hasCreep(TilePosition(x, y)))
					{
						return false;
					}
				}
			}
		}
	}

	//Step 7. Check if we can build structure at the selected spot
	/*if (!Broodwar->canBuildHere(worker, buildSpot, toBuild, true))
	{
		return false;
	}*/

	//Step 8: If detector, check if spot is already covered by a detector
	/*if (toBuild.isDetector())
	{
		if (!suitableForDetector(buildSpot))
		{
			return false;
		}
	}*/

	//All passed. It is possible to build here.
	return true;
}

TilePosition BuildingPlacer::findBuildSpot(UnitType toBuild)
{
	range = 40;
	if (toBuild.getID() == UnitTypes::Protoss_Pylon.getID()) range = 80;

	//Refinery
	if (toBuild.isRefinery())
	{
		//Use refinery method
		return findRefineryBuildSpot(toBuild, Broodwar->self()->getStartLocation());
	}

	//If we find unpowered buildings, build a Pylon there
	if (BaseAgent::isOfType(toBuild, UnitTypes::Protoss_Pylon))
	{
		vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* agent = agents.at(i);
			if (agent->isAlive())
			{
				Unit* cUnit = agent->getUnit();
				if (cUnit->isUnpowered())
				{
					//Broodwar->printf("Build pylon at unpowered building %s", cUnit->getType().getName().c_str());
					TilePosition spot = findBuildSpot(toBuild, cUnit->getTilePosition());
					return spot;
				}
			}
		}
	}

	/*if (BaseAgent::isOfType(toBuild, UnitTypes::Protoss_Pylon))
	{
		if (AgentManager::getInstance()->countNoUnits(UnitTypes::Protoss_Pylon) > 0)
		{
			TilePosition cp = Commander::getInstance()->findChokePoint();
			if (cp.x() != -1)
			{
				if (!Broodwar->hasPower(cp, UnitTypes::Protoss_Cybernetics_Core))
				{
					if (AgentManager::getInstance()->noInProduction(UnitTypes::Protoss_Pylon) == 0)
					{
						TilePosition spot = findBuildSpot(toBuild, cp);
						return spot;
					}
				}
			}
		}
	}*/

	//Build near chokepoints: Bunker, Photon Cannon, Creep Colony
	if (BaseAgent::isOfType(toBuild, UnitTypes::Terran_Bunker) || BaseAgent::isOfType(toBuild, UnitTypes::Protoss_Photon_Cannon) || BaseAgent::isOfType(toBuild, UnitTypes::Zerg_Creep_Colony))
	{
		TilePosition cp = Commander::getInstance()->findChokePoint();
		if (cp.x() != -1)
		{
			TilePosition spot = findBuildSpot(toBuild, cp);
			return spot;
		}
	}

	//Base buildings.
	if (toBuild.isResourceDepot())
	{
		TilePosition start = ExplorationManager::getInstance()->searchExpansionSite();
		if (start.x() != -1)
		{
			//Expansion site found. Build close to it.
			TilePosition spot = findBuildSpot(toBuild, start);
			return spot;
		}
	}

	//General building. Search for spot around bases
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	if (isDefenseBuilding(toBuild))
	{
		//Defense building. Search backwards to find the last base.
		for (int i = (int)agents.size() - 1; i >= 0; i--)
		{
			BaseAgent* agent = agents.at(i);
			if (agent->isAlive() && agent->getUnitType().isResourceDepot() && !baseUnderConstruction(agent))
			{
				TilePosition start = agent->getUnit()->getTilePosition();
				TilePosition bSpot = findBuildSpot(toBuild, start);
				if (bSpot.x() != -1)
				{
					//Spot found, return it.
					return bSpot;
				}
			}
		}
	}
	else
	{
		//Standard building. Search forwards to find the main base.
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* agent = agents.at(i);
			if (agent->isAlive() && agent->getUnitType().isResourceDepot() && !baseUnderConstruction(agent))
			{
				TilePosition start = agent->getUnit()->getTilePosition();
				TilePosition bSpot = findBuildSpot(toBuild, start);
				if (bSpot.x() != -1)
				{
					//Spot found, return it.
					return bSpot;
				}
			}
		}
	}

	return TilePosition(-1, -1);
}

bool BuildingPlacer::isDefenseBuilding(UnitType toBuild)
{
	int bId = toBuild.getID();
	if (bId == UnitTypes::Terran_Bunker.getID()) return true;
	if (bId == UnitTypes::Protoss_Photon_Cannon.getID()) return true;
	if (bId == UnitTypes::Zerg_Creep_Colony.getID()) return true;

	return false;
}	


bool BuildingPlacer::baseUnderConstruction(BaseAgent* base)
{
	if (Constructor::isTerran())
	{
		return base->getUnit()->isBeingConstructed();
	}
	if (Constructor::isProtoss())
	{
		return base->getUnit()->isBeingConstructed();
	}
	if (Constructor::isZerg())
	{
		if (base->isOfType(UnitTypes::Zerg_Hatchery))
		{
			return base->getUnit()->isBeingConstructed();
		}
	}
	return false;
}

TilePosition BuildingPlacer::findSpotAtSide(UnitType toBuild, TilePosition start, TilePosition end)
{
	int dX = end.x() - start.x();
	if (dX != 0) dX = 1;
	int dY = end.y() - start.y();
	if (dY != 0) dY = 1;

	TilePosition cPos = start;
	bool done = false;
	while (!done) 
	{
		if (canBuildAt(toBuild, cPos)) return cPos;
		int cX = cPos.x() + dX;
		int cY = cPos.y() + dY;
		cPos = TilePosition(cX, cY);
		if (cPos.x() == end.x() && cPos.y() == end.y()) done = true;
	}

	return TilePosition(-1, -1);
}

bool BuildingPlacer::canBuildAt(UnitType toBuild, TilePosition pos)
{
	int maxW = w - toBuild.tileWidth() - 1;
	int maxH = h - toBuild.tileHeight() - 1;

	//Out of bounds check
	if (pos.x() >= 0 && pos.x() < maxW && pos.y() >= 0 && pos.y() < maxH)
	{
		if (canBuild(toBuild, pos))
		{
			return true;
		}
	}
	return false;
}

TilePosition BuildingPlacer::findBuildSpot(UnitType toBuild, TilePosition start)
{
	//Check start pos
	if (canBuildAt(toBuild, start)) return start;

	//Search outwards
	bool found = false;
	int cDiff = 1;
	TilePosition spot = TilePosition(-1, -1);
	while (!found) 
	{
		//Top
		TilePosition s = TilePosition(start.x() - cDiff, start.y() - cDiff);
		TilePosition e = TilePosition(start.x() + cDiff, start.y() - cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		//Bottom
		s = TilePosition(start.x() - cDiff, start.y() + cDiff);
		e = TilePosition(start.x() + cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		//Left
		s = TilePosition(start.x() - cDiff, start.y() - cDiff);
		e = TilePosition(start.x() - cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		//Right
		s = TilePosition(start.x() + cDiff, start.y() - cDiff);
		e = TilePosition(start.x() + cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		cDiff++;
		if (cDiff > range) found = true;
	}
	
	return spot;
}

void BuildingPlacer::addConstructedBuilding(Unit* unit)
{
	if (unit->getType().isAddon())
	{
		//Addons are handled by their main buildings
		return;
	}

	Corners c = getCorners(unit);
	fill(c);
}

void BuildingPlacer::buildingDestroyed(Unit* unit)
{
	if (unit->getType().isAddon())
	{
		//Addons are handled by their main buildings
		return;
	}

	Corners c = getCorners(unit);
	clear(c);
}

void BuildingPlacer::fill(Corners c)
{
	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == BUILDABLE)
				{
					cover_map[x][y] = BLOCKED;
				}
				if (cover_map[x][y] == TEMPBLOCKED)
				{
					cover_map[x][y] = BLOCKED;
				}
			}
		}
	}
}

void BuildingPlacer::fillTemp(UnitType toBuild, TilePosition buildSpot)
{
	Corners c = getCorners(toBuild, buildSpot);

	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == BUILDABLE)
				{
					cover_map[x][y] = TEMPBLOCKED;
				}
			}
		}
	}
}

void BuildingPlacer::clear(Corners c)
{
	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == BLOCKED)
				{
					cover_map[x][y] = BUILDABLE;
				}
				if (cover_map[x][y] == TEMPBLOCKED)
				{
					cover_map[x][y] = BUILDABLE;
				}
			}
		}
	}
}

void BuildingPlacer::clearTemp(UnitType toBuild, TilePosition buildSpot)
{
	if (buildSpot.x() == -1)
	{
		return;
	}

	Corners c = getCorners(toBuild, buildSpot);

	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == TEMPBLOCKED)
				{
					cover_map[x][y] = BUILDABLE;
				}
			}
		}
	}
}

Corners BuildingPlacer::getCorners(Unit* unit)
{
	return getCorners(unit->getType(), unit->getTilePosition());
}

Corners BuildingPlacer::getCorners(UnitType type, TilePosition center)
{
	int x1 = center.x();
	int y1 = center.y();
	int x2 = x1 + type.tileWidth() - 1;
	int y2 = y1 + type.tileHeight() - 1;

	int margin = 0;
	if (type.canProduce()) margin = 1;
	if (type.getID() == UnitTypes::Terran_Bunker.getID()) margin = 1;

	x1 -= margin;
	x2 += margin;
	y1 -= margin;
	y2 += margin;

	//Special case: Terran Addon buildings
	//Add 2 extra spaces to the right to make space for the addons.
	if (BaseAgent::isOfType(type, UnitTypes::Terran_Factory) || BaseAgent::isOfType(type, UnitTypes::Terran_Starport) || BaseAgent::isOfType(type, UnitTypes::Terran_Command_Center) || BaseAgent::isOfType(type, UnitTypes::Terran_Science_Facility))
	{
		x2 += 2;
	}

	Corners c;
	c.x1 = x1;
	c.y1 = y1;
	c.x2 = x2;
	c.y2 = y2;

	return c;
}

TilePosition BuildingPlacer::findRefineryBuildSpot(UnitType toBuild, TilePosition start)
{
	TilePosition buildSpot = findClosestGasWithoutRefinery(toBuild, start);
	if (buildSpot.x() >= 0)
	{
		BaseAgent* base = AgentManager::getInstance()->getClosestBase(buildSpot);
		if (base == NULL)
		{
			Broodwar->printf("No base found");
			return TilePosition(-1,-1);
		}
		else
		{
			double dist = buildSpot.getDistance(base->getUnit()->getTilePosition());
			if (dist >= 14) 
			{
				//Broodwar->printf("Base too far away %d", (int)dist);
				return TilePosition(-1,-1);
			}
		}

	}
	return buildSpot;
}

TilePosition BuildingPlacer::findClosestGasWithoutRefinery(UnitType toBuild, TilePosition start)
{
	TilePosition bestSpot = TilePosition(-1,-1);
	double bestDist = -1;
	TilePosition home = Broodwar->self()->getStartLocation();
	Unit* worker = findWorker(start);

	for(int i = 0 ; i < w ; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (cover_map[i][j] == GAS)
			{
				TilePosition cPos = TilePosition(i,j);

				bool ok = true;
				vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
				for (int i = 0; i < (int)agents.size(); i++)
				{
					Unit* unit = agents.at(i)->getUnit();
					if (unit->getType().isRefinery())
					{
						double dist = unit->getTilePosition().getDistance(cPos);
						if (dist <= 2)
						{
							ok = false;
						}
					}
				}
				if (ok)
				{
					if (ExplorationManager::canReach(home, cPos))
					{
						BaseAgent* agent = AgentManager::getInstance()->getClosestBase(cPos);
						double dist = agent->getUnit()->getTilePosition().getDistance(cPos);
						if (bestDist == -1 || dist < bestDist)
						{
							bestDist = dist;
							bestSpot = cPos;
						}
					}
				}
			}
		}
	}

	return bestSpot;
}

TilePosition BuildingPlacer::searchRefinerySpot()
{
	for(int i = 0 ; i < w ; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (cover_map[i][j] == GAS)
			{
				TilePosition cPos = TilePosition(i,j);

				bool found = false;
				vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
				for (int i = 0; i < (int)agents.size(); i++)
				{
					if (agents.at(i)->getUnitType().isRefinery())
				{
						double dist = agents.at(i)->getUnit()->getTilePosition().getDistance(cPos);
						TilePosition uPos = agents.at(i)->getUnit()->getTilePosition();
						if (dist <= 2)
				{
							found = true;
							break;
						}
					}
				}

				if (!found)
				{
					BaseAgent* agent = AgentManager::getInstance()->getClosestBase(cPos);
					if (agent != NULL)
					{
						TilePosition bPos = agent->getUnit()->getTilePosition();
						double dist = bPos.getDistance(cPos);

						if (dist < 15)
						{
							if (ExplorationManager::canReach(bPos, cPos))
							{
								return cPos;
							}			
						}
					}
				}
			}
		}
	}

	return TilePosition(-1, -1);
}

TilePosition BuildingPlacer::findExpansionSite()
{
	UnitType baseType = Broodwar->self()->getRace().getCenter();
	double bestDist = 100000;
	TilePosition bestPos = TilePosition(-1, -1);
	
	//Iterate through all base locations
	for(set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin(); i!= BWTA::getBaseLocations().end(); i++)
	{
		TilePosition pos = (*i)->getTilePosition();
		bool taken = false;
		
		//Check if own buildings are close
		vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		int noBases = 0;
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* agent = agents.at(i);
			if (agent->isAlive() && agent->getUnitType().isResourceDepot())
			{
				double dist = pos.getDistance(agent->getUnit()->getTilePosition());
				if (dist <= 12)
				{
					noBases++;
				}
			}
		}
		
		if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
		{
			//if (noBases >= 2) taken = true;
			if (noBases >= 1) taken = true;
		}
		else
		{
			if (noBases >= 1) taken = true;
		}
		
		//Check if enemy buildings are close
		int eCnt = ExplorationManager::getInstance()->spottedBuildingsWithinRange(pos, 20);
		if (eCnt > 0)
		{
			taken = true;
		}

		//Not taken, calculate ground distance
		if (!taken)
		{
			if (ExplorationManager::canReach(Broodwar->self()->getStartLocation(), pos))
			{
				double dist = mapData.getDistance(Broodwar->self()->getStartLocation(), pos);
				if (dist <= bestDist)
				{
					bestDist = dist;
					bestPos = pos;
				}
			}
		}
	}

	//Don't build expansions too far away!
	BaseAgent* base = AgentManager::getInstance()->getClosestBase(bestPos);
	double d = base->getUnit()->getTilePosition().getDistance(bestPos);
	if (d >= 140)
	{
		Broodwar->printf("Expansion site too far away: %d", (int)d);
		return TilePosition(-1, -1);
	}

	return bestPos;
}

Unit* BuildingPlacer::findClosestMineral(TilePosition workerPos)
{
	Unit* mineral = NULL;
	double bestDist = 10000;

	for(set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin(); i!= BWTA::getBaseLocations().end(); i++)
	{
		TilePosition pos = (*i)->getTilePosition();
		double cDist = pos.getDistance(workerPos);
		if (cDist < bestDist)
		{
			//Find closest base
			BaseAgent* base = AgentManager::getInstance()->getClosestBase(pos);
			double dist = pos.getDistance(base->getUnit()->getTilePosition());
			if (dist <= 12)
			{
				//We have a base near this base location
				//Check if we have minerals available
				Unit* cMineral = hasMineralNear(pos);
				if (cMineral != NULL)
				{
					mineral = cMineral;
					bestDist = cDist;
				}
			}
		}
	}

	//We have no base with minerals, do nothing
	return mineral;
}

Unit* BuildingPlacer::hasMineralNear(TilePosition pos)
{
	for(set<Unit*>::iterator m = Broodwar->getMinerals().begin(); m != Broodwar->getMinerals().end(); m++)
	{
		if ((*m)->exists() && (*m)->getResources() > 0)
		{
			double dist = pos.getDistance((*m)->getTilePosition());
			if (dist <= 10)
			{
				return (*m);
			}
		}
	}
	return NULL;
}

bool BuildingPlacer::suitableForDetector(TilePosition pos)
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		UnitType type = agent->getUnitType();
		if (agent->isAlive() && type.isDetector() && type.isBuilding())
		{
			double dRange = type.sightRange() * 1.6;
			double dist = agent->getUnit()->getPosition().getDistance(Position(pos));
			if (dist <= dRange)
			{
				return false;
			}
		}
	}
	return true;
}

void BuildingPlacer::debug()
{
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (cover_map[x][y] == TEMPBLOCKED)
			{
				Broodwar->drawBoxMap(x*32,y*32,x*32+31,y*32+31,Colors::Green,false);
			}
			if (cover_map[x][y] == BLOCKED)
			{
				Broodwar->drawBoxMap(x*32,y*32,x*32+31,y*32+31,Colors::Brown,false);
			}
		}
	}
}
