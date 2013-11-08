#include "AIloop.h"
#include "BuildingPlacer.h"
#include "Profiler.h"
#include "Upgrader.h"
#include "ResourceManager.h"
#include "NavigationAgent.h"
#include <sstream>
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "Commander.h"
#include "Constructor.h"
#include "StrategySelector.h"


AIloop::AIloop()
{
	unitDebug = false;
	pfDebug = false;
	bpDebug = false;
	debugSQ = -1;
	debug = true;

	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++) 
	{
		AgentManager::getInstance()->addAgent(*i);
	}
}

AIloop::~AIloop()
{
	
}

void AIloop::toggleDebug()
{
	debug = !debug;
}

void AIloop::toggleUnitDebug()
{
	unitDebug = !unitDebug;
}

void AIloop::togglePFDebug()
{
	pfDebug = !pfDebug;
}

void AIloop::toggleBPDebug()
{
	bpDebug = !bpDebug;
}

void AIloop::setDebugSQ(int squadID)
{
	debugSQ = squadID;
}

void AIloop::computeActions()
{
	if (AgentManager::getInstance()->countNoBases() == 0)
	{
		if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
		{
			int no = AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Lair) + AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Hive);
			if (no == 0)
			{
				//No bases left. Leave game.
				Broodwar->leaveGame();
				return;
			}
		}
		else
		{
			//No bases left. Leave game.
			Broodwar->leaveGame();
			return;
		}
	}	

	Profiler::getInstance()->start("OnFrame_AgentManager");
	AgentManager::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_AgentManager");
	Profiler::getInstance()->start("OnFrame_Constructor");
	Constructor::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_Constructor");
	Profiler::getInstance()->start("OnFrame_Commander");
	Commander::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_Commander");
	Profiler::getInstance()->start("OnFrame_ExplorationManager");
	ExplorationManager::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_ExplorationManager");
}

void AIloop::addUnit(Unit* unit)
{
	AgentManager::getInstance()->addAgent(unit);

	//Remove from buildorder if this is a building
	if (unit->getType().isBuilding())
	{
		Constructor::getInstance()->unlock(unit->getType());
	}
}

void AIloop::morphUnit(Unit* unit)
{
	AgentManager::getInstance()->morphDrone(unit);
	Constructor::getInstance()->unlock(unit->getType());
}

void AIloop::unitDestroyed(Unit* unit)
{
	if (unit->getPlayer()->getID() == Broodwar->self()->getID())
	{
		AgentManager::getInstance()->removeAgent(unit);
		if (unit->getType().isBuilding())
		{
			Constructor::getInstance()->buildingDestroyed(unit);
		}

		//Assist workers under attack
		if (unit->getType().isWorker())
		{
			Commander::getInstance()->assistWorker(AgentManager::getInstance()->getAgent(unit->getID()));
		}

		//Update dead score
		if (unit->getType().canMove())
		{
			Commander::getInstance()->ownDeadScore += unit->getType().destroyScore();
		}

		AgentManager::getInstance()->cleanup();
	}
	if (unit->getPlayer()->getID() != Broodwar->self()->getID() && !unit->getPlayer()->isNeutral())
	{
		ExplorationManager::getInstance()->unitDestroyed(unit);
		Commander::getInstance()->enemyDeadScore += unit->getType().destroyScore();
	}
}

void AIloop::show_debug()
{
	if (debug)
	{
		//Show timer
		stringstream ss;
		ss << "\x0FTime: ";
		ss << Broodwar->elapsedTime() / 60;
		ss << ":";
		int sec = Broodwar->elapsedTime() % 60;
		if (sec < 10) ss << "0";
		ss << sec;

		Broodwar->drawTextScreen(100,5, ss.str().c_str());
		//

		StrategySelector::getInstance()->printInfo();

		if (bpDebug)
		{
			BuildingPlacer::getInstance()->debug();
		}
		drawTerrainData();

		Commander::getInstance()->debug_showGoal();

		vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (agents.at(i)->isBuilding()) agents.at(i)->debug_showGoal();
		}
		
		//Show goal info for selected units
		set<Unit*> units = Broodwar->getSelectedUnits();
		if ((int)units.size() > 0) 
		{
			for(set<Unit*>::const_iterator i=units.begin();i!=units.end();i++)
			{
				int unitID = (*i)->getID();
				BaseAgent* agent = AgentManager::getInstance()->getAgent(unitID);
				if (agent != NULL && agent->isAlive())
				{
					agent->debug_showGoal();
				}
			}
		}

		if (pfDebug) 
		{
			//If we have any unit selected, use that to show PFs.
			set<Unit*> units = Broodwar->getSelectedUnits();
			if ((int)units.size() > 0) 
			{
				int unitID = (*units.begin())->getID();
				BaseAgent* agent = AgentManager::getInstance()->getAgent(unitID);
				if (agent != NULL) 
				{
					NavigationAgent::getInstance()->displayPF(agent);
				}
			}
		}

		if (unitDebug) 
		{
			//If we have any unit selected, show unit info.
			set<Unit*> units = Broodwar->getSelectedUnits();
			if ((int)units.size() > 0) 
			{
				int unitID = (*units.begin())->getID();
				BaseAgent* agent = AgentManager::getInstance()->getAgent(unitID);
				if (agent != NULL) 
				{
					agent->printInfo();
				}
			}
		}

		if (debugSQ >= 0)
		{
			Squad* squad = Commander::getInstance()->getSquad(debugSQ);
			if (squad != NULL) 
			{
				squad->printInfo();
			}
		}

		Upgrader::getInstance()->printInfo();
		//ExplorationManager::getInstance()->printInfo();
		Commander::getInstance()->printInfo();
	}
}

void AIloop::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
	{
		TilePosition p=(*i)->getTilePosition();
		Position c=(*i)->getPosition();

		//Draw a progress bar at each resource
		for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
		{
			if ((*j)->getResources() > 0)
			{
				
				int total = (*j)->getInitialResources();
				int done = (*j)->getResources();

				int w = 60;
				int h = 64;

				//Start 
				Position s = Position((*j)->getPosition().x() - w/2 + 2, (*j)->getPosition().y() - 4);
				//End
				Position e = Position(s.x() + w, s.y() + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x() + prg, s.y() +  8);

				Broodwar->drawBoxMap(s.x(),s.y(),e.x(),e.y(),Colors::Orange,false);
				Broodwar->drawBoxMap(s.x(),s.y(),p.x(),p.y(),Colors::Orange,true);
			}
		}
	}

	if (bpDebug)
	{
		//we will iterate through all the regions and draw the polygon outline of it in white.
		for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
		{
			BWTA::Polygon p=(*r)->getPolygon();
			for(int j=0;j<(int)p.size();j++)
			{
				Position point1=p[j];
				Position point2=p[(j+1) % p.size()];
				Broodwar->drawLineMap(point1.x(),point1.y(),point2.x(),point2.y(),Colors::Orange);
			}
		}

		//we will visualize the chokepoints with yellow lines
		for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
		{
			for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
			{
				Position point1=(*c)->getSides().first;
				Position point2=(*c)->getSides().second;
				Broodwar->drawLineMap(point1.x(),point1.y(),point2.x(),point2.y(),Colors::Yellow);
			}
		}
	}

	//locate zerg eggs and draw progress bars
	if (Constructor::isZerg())
	{
		for(std::set<Unit*>::const_iterator u = Broodwar->self()->getUnits().begin(); u != Broodwar->self()->getUnits().end(); u++)
		{
			if ((*u)->getType().getID() == UnitTypes::Zerg_Egg.getID() || (*u)->getType().getID() == UnitTypes::Zerg_Lurker_Egg.getID() || (*u)->getType().getID() == UnitTypes::Zerg_Cocoon.getID())
			{
				int total = (*u)->getBuildType().buildTime();
				int done = total - (*u)->getRemainingBuildTime();
				
				int w = (*u)->getType().tileWidth() * 32;
				int h = (*u)->getType().tileHeight() * 32;

				//Start 
				Position s = Position((*u)->getPosition().x() - w/2, (*u)->getPosition().y() - 4);
				//End
				Position e = Position(s.x() + w, s.y() + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x() + prg, s.y() +  8);

				Broodwar->drawBoxMap(s.x(),s.y(),e.x(),e.y(),Colors::Blue,false);
				Broodwar->drawBoxMap(s.x(),s.y(),p.x(),p.y(),Colors::Blue,true);
			}
		}
	}
}
