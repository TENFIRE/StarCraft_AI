#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

struct CTokens {
	string key;
	string value;
};

/** This class reads and parses the bthai-config.txt file, and contain methods for getting
 * parameters from the file.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Config {

private:
	string botName;
	string version;
	string info;
	int w;

	static Config* instance;
	
	Config();

public:
	~Config();

	/** Returns class instance. */
	static Config* getInstance();

	/** Returns the name of the bot as specified in the config file. */
	string getBotName();

	/** Returns the current bot version. */
	string getVersion();

	/** Displays bot name in the game window. */
	void displayBotName();
};

#endif
