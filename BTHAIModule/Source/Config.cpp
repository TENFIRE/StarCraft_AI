#include "Config.h"
#include "Constructor.h"
#include "ExplorationManager.h"
#include <sstream>

Config* Config::instance = NULL;

Config::Config()
{
	version = "3.31";
	botName = "BTHAI";

	stringstream ss;
	ss << "\x1C";
	ss << botName;
	ss << " ";
	ss << version;

	info = ss.str();

	int l = botName.length()+version.length();
	w = 7+l*6;
}

Config::~Config()
{
	delete instance;
}

Config* Config::getInstance()
{
	if (instance == NULL)
	{
		instance = new Config();
	}
	return instance;
}

void Config::displayBotName()
{
	if (Broodwar->getFrameCount() >= 10)
	{
		Broodwar->drawBoxScreen(3,3,w,20,Colors::Black,true);
		Broodwar->drawTextScreen(7,5, info.c_str());
	}
}

string Config::getVersion()
{
	return version;
}

string Config::getBotName()
{
	return botName;
}
