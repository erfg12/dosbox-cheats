/*
 *  Created by Jacob Fliss 2018
 *
 *  Version: 0.0.1
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include <sstream>


#include "dosbox.h"
#include "debug.h"
#include "support.h"
#include "setup.h"
#include "cross.h"
#include "cheats.h"
#include "control.h"
#include "timer.h"

using namespace std;

std::vector<std::string> explode(std::string const & s, char delim)
{
	std::vector<std::string> result;
	std::istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim); )
	{
		result.push_back(std::move(token));
	}

	return result;
}

int hexToInt(string s) {
	char * p;

	long n = std::stol(s, 0, 16);

	if (*p != 0)
		return 0;
	else
		return n;
}

string curCheat;
int tick = 0;
cheat::codes c;

/// Description: read cheats from file, store in dictionary
/// Format: TOGGLE:NAME:TYPE-OFFSET:BYTES
/// toggle = off or on
/// types = a(active) and o(once)
/// bytes = a0,0b,c0
void cheat::ParseCheatFile(const char *game) {
	string s(game);
	string path;
	Cross::GetPlatformConfigDir(path);
	string file = path + "cheats\\" + s + ".cht";
	ifstream in(file);
	if (!in) return;
	if (curCheat.compare(string(game)) == 0) return; // already loaded

	c.clear(); // clear old codes

	LOG_MSG("CHEAT: Loading cheats from file %s", file.c_str());

	string gegevens;
	while (getline(in, gegevens)) {
		trim(gegevens);
		if (!gegevens.size()) {
			continue;
		}
		vector<string> disect = explode(gegevens, ':');
		if (disect[0].compare("on") == 0) {
			c[disect[2]] = disect[3];
			LOG_MSG("CHEAT: %s ON", disect[1].c_str());
		}
		else {
			LOG_MSG("CHEAT: %s OFF", disect[1].c_str());
		}
	}
	curCheat = string(game);
	cheat::WriteCodeType("o");
}

/// Description: get active or once code types
void cheat::WriteCodeType(string type) {
	//LOG_MSG("CHEAT: Writing %s codes", type.c_str());
	for (const auto& kv : c) {
		vector<string> d = explode(kv.first, '-');
		if (d[0].compare(type) == 0) { // only use types we're looking for
			// now check for byte array, if true iteratate through byte array and adjust offset +1
			int startAddr = hexToInt(d[1]);
			if (kv.second.find(",") != string::npos) {
				vector<string> b = explode(kv.second, ',');
				//LOG_MSG("CHEAT: Byte array %s", kv.second.c_str());
				for (string t : b) {
					int byte = hexToInt(t);
					//LOG_MSG("CHEAT: Writing byte %i to %i", byte, startAddr);
					phys_writeb(startAddr++, byte);
				}
			}
			else { // else write single byte
				int byte = hexToInt(kv.second);
				//LOG_MSG("CHEAT: Writing byte %i to %i", byte, startAddr);
				phys_writeb(startAddr, byte);
			}
		}
	}
}

static void CHEATS_TickHandler(void) {
	//LOG_MSG("CHEAT: TickHandler");
	if (tick == 1000) {
		cheat::WriteCodeType("a");
		tick = 0;
	}
	tick++;
}

void CHEATS_Init(Section* sec) {
	//LOG_MSG("CHEAT: Initiated");
	TIMER_AddTickHandler(&CHEATS_TickHandler);
}
