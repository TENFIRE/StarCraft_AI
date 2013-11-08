#include "BTHAIModule.h"
#include "Constructor.h"
#include "ExplorationManager.h"
#include "BuildingPlacer.h"
#include "Commander.h"
#include "Pathfinder.h"
#include "Upgrader.h"
#include "ResourceManager.h"
#include "Profiler.h"
#include "NavigationAgent.h"
#include "Config.h"
#include "StrategySelector.h"
#include "DefenseLocator.h"
#include <Shlwapi.h>

using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
bool leader = false;

void BTHAIModule::onStart() 
{
	Profiler::getInstance()->start("OnInit");

	//Needed for text commands to work
	Broodwar->enableFlag(Flag::UserInput);
	//Set speed
	speed = 8;
	Broodwar->setLocalSpeed(speed);

	//Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);
	
	//Analyze map using BWTA
	BWTA::readMap();
	analyzed=false;
	analysis_just_finished=false;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL); //Threaded version
	AnalyzeThread();
	
	profile = false;

	//Init our singleton agents
	BuildingPlacer::getInstance();
	Constructor::getInstance();
	Upgrader::getInstance();
	ResourceManager::getInstance();
	Pathfinder::getInstance();
	loop = new AIloop();
	
	if (Broodwar->isReplay()) 
	{
		Broodwar->printf("The following players are in this replay:");
		for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
			}
		}
	}

    //Add the units we have from start to agent manager
	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++) 
	{
		AgentManager::getInstance()->addAgent(*i);
	}

	//Broodwar->printf("BTHAI %s (%s)", VERSION.c_str(), Broodwar->self()->getRace().getName().c_str());

	running = true;

	//Broodwar->setCommandOptimizationLevel(0); //0--3

	//Debug mode. Active panels.
	Commander::getInstance()->toggleSquadsDebug();
	Commander::getInstance()->toggleBuildplanDebug();
	Upgrader::getInstance()->toggleDebug();
	speed = 4;
	Broodwar->setLocalSpeed(speed);
	loop->toggleUnitDebug();
	//loop->toggleBPDebug();
	//End Debug mode

	Profiler::getInstance()->end("OnInit");
}

void BTHAIModule::gameStopped()
{
	Pathfinder::getInstance()->stop();
	Profiler::getInstance()->dumpToFile();
	running = false;

	//Delete singletons
	delete Pathfinder::getInstance();
	delete Profiler::getInstance();
	delete AgentManager::getInstance();
	delete BuildingPlacer::getInstance();
	delete ResourceManager::getInstance();
	delete Constructor::getInstance();
	delete Commander::getInstance();
	delete ExplorationManager::getInstance();
	delete NavigationAgent::getInstance();
	delete StrategySelector::getInstance();
	delete DefenseLocator::getInstance();
	delete loop;
}

void BTHAIModule::onEnd(bool isWinner) 
{
	if (Broodwar->elapsedTime()/60 < 4) return;

	int win = 0;
	if (isWinner) win = 1;
	if (Broodwar->elapsedTime()/60 >= 80) win = 2;
	
	StrategySelector::getInstance()->addResult(win);
	StrategySelector::getInstance()->saveStats();

	gameStopped();
}

void BTHAIModule::onFrame() 
{
	Profiler::getInstance()->start("OnFrame");

	if (!running) 
	{
		//Game over. Do nothing.
		return;
	}
	if (!Broodwar->isInGame()) 
	{
		//Not in game. Do nothing.
		gameStopped();
		return;
	}
	if (Broodwar->isReplay()) 
	{
		//Replay. Do nothing.
		return;
	}
	
	if (Broodwar->elapsedTime() / 60 >= 81)
	{
		//Stalled game. Leave it.
		Broodwar->leaveGame();
		return;
	}

	loop->computeActions();
	loop->show_debug();

	Config::getInstance()->displayBotName();

	Profiler::getInstance()->end("OnFrame");

	if (profile) Profiler::getInstance()->showAll();
}

void BTHAIModule::onSendText(std::string text) 
{
	if (text=="/a") 
	{
		Commander::getInstance()->forceAttack();
	}
	else if(text=="/p") 
	{
		profile = !profile;
	}
	else if(text=="d")
	{
		loop->toggleDebug();
	}
	else if(text=="pf") 
	{
		loop->togglePFDebug();
	}
	else if(text=="bp") 
	{
		loop->toggleBPDebug();
	}
	else if (text.substr(0, 2)=="sq") 
	{
		if (text=="sq")
		{
			loop->setDebugSQ(-1);
		}
		else
		{
			int id = atoi(&text[2]);
			loop->setDebugSQ(id);
		}
	}
	else if (text=="+") 
	{
		speed -= 4;
		if (speed < 0) 
		{
			speed = 0;
		}
		Broodwar->printf("Changed game speed: %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="++") 
	{
		speed = 0;
		Broodwar->printf("Changed game speed: %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="-") 
	{
		speed += 4;
		Broodwar->printf("Changed game speed: %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="--") 
	{
		speed = 24;
		Broodwar->printf("Changed game speed: %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="t") 
	{
		Upgrader::getInstance()->toggleDebug();
	}
	else if (text=="s") 
	{
		Commander::getInstance()->toggleSquadsDebug();
	}
	else if (text=="b") 
	{
		Commander::getInstance()->toggleBuildplanDebug();
	}
	else if (text=="i") 
	{
		loop->toggleUnitDebug();
	}
	else 
	{
		Broodwar->printf("You typed '%s'!",text.c_str());
	}
}

void BTHAIModule::onReceiveText(BWAPI::Player* player, std::string text) 
{
	Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void BTHAIModule::onPlayerLeft(BWAPI::Player* player) 
{
	
}

void BTHAIModule::onNukeDetect(BWAPI::Position target) 
{
	if (target != Positions::Unknown) 
	{
		TilePosition t = TilePosition(target);
		Broodwar->printf("Nuclear Launch Detected at (%d,%d)",t.x(),t.y());
	}
	else
	{
		Broodwar->printf("Nuclear Launch Detected");
	}
}

void BTHAIModule::onUnitDiscover(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	if (unit->getPlayer()->getID() != Broodwar->self()->getID()) 
	{
		if (!unit->getPlayer()->isNeutral() && !unit->getPlayer()->isAlly(Broodwar->self()))
		{
			ExplorationManager::getInstance()->addSpottedUnit(unit);
		}
	}
}

void BTHAIModule::onUnitEvade(BWAPI::Unit* unit) 
{
	
}

void BTHAIModule::onUnitShow(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	if (unit->getPlayer()->getID() != Broodwar->self()->getID()) 
	{
		if (!unit->getPlayer()->isNeutral() && !unit->getPlayer()->isAlly(Broodwar->self()))
		{
			ExplorationManager::getInstance()->addSpottedUnit(unit);
		}
	}
}

void BTHAIModule::onUnitHide(BWAPI::Unit* unit) 
{
	
}

void BTHAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	if (unit->getPlayer()->getID() == Broodwar->self()->getID())
	{
		loop->addUnit(unit);
	}
}

void BTHAIModule::onUnitDestroy(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	loop->unitDestroyed(unit);
}

void BTHAIModule::onUnitMorph(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	if (unit->getPlayer()->getID() == Broodwar->self()->getID()) 
	{
		if (Constructor::isZerg())
		{
			loop->morphUnit(unit);
		}
		else
		{
			loop->addUnit(unit);
		}
	}
}

void BTHAIModule::onUnitRenegade(BWAPI::Unit* unit) 
{
	
}

void BTHAIModule::onSaveGame(std::string gameName) 
{
	Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();
	
	analyzed = true;
	analysis_just_finished = true;
	return 0;
}

bool BTHAITournamentModule::onAction(int actionType, void *parameter)
{
	switch ( actionType )
	{
		case Tournament::SendText:
		case Tournament::Printf:
			return true;
		case Tournament::EnableFlag:
			switch ( *(int*)parameter )
			{
				case Flag::CompleteMapInformation:
				case Flag::UserInput:
					// Disallow these two flags
					return false;
			}
			// Allow other flags if we add more that don't affect gameplay specifically
			return true;
		case Tournament::LeaveGame:
		case Tournament::PauseGame:
		case Tournament::RestartGame:
		case Tournament::ResumeGame:
		case Tournament::SetFrameSkip:
		case Tournament::SetGUI:
		case Tournament::SetLocalSpeed:
		case Tournament::SetMap:
			return false; // Disallow these actions
		case Tournament::ChangeRace:
		case Tournament::SetLatCom:
		case Tournament::SetTextSize:
			return true; // Allow these actions
		case Tournament::SetCommandOptimizationLevel:
			return *(int*)parameter > MINIMUM_COMMAND_OPTIMIZATION; // Set a minimum command optimization level 
                                                            // to reduce APM with no action loss
		default:
			break;
	}
	return true;
}

void BTHAITournamentModule::onFirstAdvertisement()
{
	leader = true;
	Broodwar->sendText("Welcome to " TOURNAMENT_NAME "!");
	Broodwar->sendText("Brought to you by " SPONSORS ".");
}
