#pragma once
#include "main.h"
#include <map>

extern bool FirstFrame;
extern GameInfo_s BuildInfo;
void ConsolePrintColor(BYTE R, BYTE G, BYTE B, const char *fmt, ...);
void HookUserMessages();
void HookEngineMessages();
void HookFunction();

enum cvar_modes{
	cvar_bad = 0,
	cvar_fake = 1,
	cvar_open = 2
};

struct m_Cvar{
	byte mode;
	string name;
	string value;
	string default;
};
void AddOrModCvar(const string line);
extern vector<m_Cvar> Cvars;
extern ptrdiff_t FindCvar(string name, vector<m_Cvar> vec_cvar);