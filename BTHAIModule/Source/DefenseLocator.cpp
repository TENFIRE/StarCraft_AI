#include "DefenseLocator.h"
#include "AgentManager.h"

DefenseLocator* DefenseLocator::instance = NULL;

DefenseLocator::DefenseLocator()
{
	//Fading Realms
	/*data.push_back(DefensePos(25,18,25,18,85,29,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(25,28,114,6,120,26,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(25,18,115,34,51,51,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(25,18,63,45,51,51,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(25,18,9,59,51,51,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(99,74,99,74,44,64,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(99,74,9,87,14,68,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(99,74,9,59,73,42,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(99,74,63,45,73,42,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
	data.push_back(DefensePos(99,74,115,34,73,42,"d05dbc04919ce95859eb4b42fcd04837b4ea5df3"));
*/
}

DefenseLocator::~DefenseLocator()
{
	instance = NULL;
}

DefenseLocator* DefenseLocator::getInstance()
{
	if (instance == NULL)
	{
		instance = new DefenseLocator();
	}
	return instance;
}

TilePosition DefenseLocator::getBaseDefensePos(string mMaphash)
{
	TilePosition pos = TilePosition(-1,-1);

	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int a = 0; a < (int)agents.size(); a++)
	{
		for (int i = 0; i < (int)data.size(); i++)
		{
			if (data.at(i).maphash == mMaphash)
			{		
				if (agents.at(a)->getUnitType().isResourceDepot() && agents.at(a)->isAlive())
				{
					TilePosition dBase = agents.at(a)->getUnit()->getTilePosition();
					TilePosition hBase = Broodwar->self()->getStartLocation();
	
					if (data.at(i).matches(hBase,dBase))
					{
						pos = data.at(i).pos;
					}
				}
			}
		}
	}

	return pos;
}
