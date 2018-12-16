#ifndef DOSBOX_CHEATS_H
#define DOSBOX_CHEATS_H

#ifndef DOSBOX_MEM_H
#include "mem.h"
#endif

#include <map>

class cheat {
public:
	static void ParseCheatFile(const char *game);
	typedef std::map<std::string, std::string> codes;
	static void WriteCodeType(std::string type);
};

void CHEATS_Init(Section* sec);

#endif
