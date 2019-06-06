#include "client.h"
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::copy
#include "minIni.h"
#include <vector>

#pragma warning(disable:4996)
extern TCHAR g_settingsFileName[MAX_PATH];
bool FirstFrame = false;
pfnUserMsgHook pMOTD;
GameInfo_t BuildInfo;
cvar_t *steamid_r;
map<string, string> g_modelsHashMap;
cvar_t *logsfiles;
cvar_t *events_block;
cvar_t *ex_thud;
cvar_t *motd_block;

vector<m_Cvar> Cvars;

void HookEngineMessages(){
	pEngineMsgBase = (PEngineMsg)offset.FindSVCMessages();
	pSVC_StuffText = HookEngineMsg("svc_stufftext", SVC_StuffText);
	pSVC_SendCvarValue = HookEngineMsg("svc_sendcvarvalue", SVC_SendCvarValue);
	pSVC_SendCvarValue2 = HookEngineMsg("svc_sendcvarvalue2", SVC_SendCvarValue2);
	pSVC_Director = HookEngineMsg("svc_director", SVC_Director);
	pSVC_VoiceInit = HookEngineMsg("svc_voiceinit", SVC_VoiceInit);
}


void ConsolePrintColor(BYTE R, BYTE G, BYTE B, const char *fmt, ...){
	va_list va_alist;
	char buf[256];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	TColor24 DefaultColor; PColor24 Ptr; Ptr = Console_TextColor; DefaultColor = *Ptr; Ptr->R = R; Ptr->G = G; Ptr->B = B; g_Engine.Con_Printf(buf); *Ptr = DefaultColor;
}

void models(){
	for (DWORD i = 0; i < 32; i++){
		player_info_s* player = g_pStudio->PlayerInfo(i);
		if (player && (lstrlenA(player->name)>1) && player->model){
			ConsolePrintColor(255, 255, 15, "NAME -> [ %s ]  | MODEL -> [ %s ]\n", player->name, player->model);
		}
	}
}
string filename;
void Set_Ticket() {
	filename = g_Engine.Cmd_Argv(1);
	ConsolePrintColor(255, 255, 255, "[ExtraMirror] Ticket set -> \"%s\"\n", filename.c_str());
}
void Credits(){
	ConsolePrintColor(255, 255, 255, "-- Thank's to");ConsolePrintColor(0, 255, 0, " [2010] Team\n");ConsolePrintColor(255, 255, 255, "-- Thank's to");
	ConsolePrintColor(0, 255, 0, " madotsuki-team < *\n");ConsolePrintColor(255, 255, 255, "-- Thank's to ");ConsolePrintColor(0, 255, 0, "or_75\n");
	ConsolePrintColor(255, 255, 255, "-- Thank's to "); ConsolePrintColor(0, 255, 0, "Juice\n");
	ConsolePrintColor(255, 255, 255, "-- Thank's to "); ConsolePrintColor(0, 255, 0, "Admrfsh\n");
	ConsolePrintColor(255, 255, 255, "-- Thank's to "); ConsolePrintColor(0, 255, 0, "Garey\n");
}
int g_blockedCmdCount, g_serverCmdCount,g_anticheckfiles;
char *g_blockedCmds[1024], *g_serverCmds[2048], *g_anticheckfiles2[2048];

struct models_replace_s{char name[32];char repl[32];};
vector<models_replace_s> models_list;
int Callback(const char *section, const char *key, const char *value, const void *userdata){
	if (lstrcmpA(section, "Models") == 0){
		models_replace_s model_d;lstrcpyA(model_d.name, key);lstrcpyA(model_d.repl, value);models_list.push_back(model_d);
	}
	return 1;
}
void Inject(){LoadLibraryA(g_Engine.Cmd_Argv(1)); }

void DumpCmd(){
	cmd_s *pCmd = g_Engine.pfnGetCmdList();
	ConsolePrintColor(255, 255, 255, "Dump Commands: \n");
	while (pCmd)
	{
		ConsolePrintColor(255,255,255, "%s \n", (char*)pCmd->name);
		pCmd = pCmd->next;
	}
}

void Reload(){
	models_list.clear();
	ini_browse(Callback,NULL,g_settingsFileName);
	memset(g_blockedCmds,0,sizeof(g_blockedCmds));
	memset(g_serverCmds, 0, sizeof(g_serverCmds));
	memset(g_anticheckfiles2, 0, sizeof(g_anticheckfiles2));
	g_blockedCmdCount = 0;
	g_serverCmdCount = 0; 
	g_anticheckfiles = 0;
	static TCHAR sKeyNames[4096*3];

	GetPrivateProfileSection(TEXT("ADetect"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	char *psKeyName4 = sKeyNames;
	while (psKeyName4[0] != '\0') { g_anticheckfiles2[g_anticheckfiles++] = strdup(psKeyName4); psKeyName4 += strlen(psKeyName4) + 1; }

	GetPrivateProfileSection(TEXT("Commands"),sKeyNames,ARRAYSIZE(sKeyNames),g_settingsFileName);
	char *psKeyName = sKeyNames;
	while (psKeyName[0]!='\0'){g_blockedCmds[g_blockedCmdCount++]=strdup(psKeyName);psKeyName+=strlen(psKeyName)+1;}

	GetPrivateProfileSection(TEXT("Send Commands"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	char *psKeyName3 = sKeyNames;
	while (psKeyName3[0] != '\0') { g_serverCmds[g_serverCmdCount++] = strdup(psKeyName3); psKeyName3 += strlen(psKeyName3) + 1; }

	GetPrivateProfileSection(TEXT("Cvars"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	char *psKeyName2 = sKeyNames;
	// Clear vector
	Cvars.clear();
	while (psKeyName2[0] != '\0'){AddOrModCvar(psKeyName2);psKeyName2 += strlen(psKeyName2) + 1;}
	TCHAR value[16];char cvarname[32];
	GetPrivateProfileString(TEXT("Settings"), TEXT("steamid"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	sprintf(cvarname, "steamid %s", value);g_Engine.pfnClientCmd(cvarname); memset(value, 0, sizeof(value)); memset(cvarname, 0, sizeof(cvarname));
	GetPrivateProfileString(TEXT("Settings"), TEXT("cust_hud"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	sprintf(cvarname, "cust_hud %s", value); g_Engine.pfnClientCmd(cvarname);memset(value, 0, sizeof(value)); memset(cvarname, 0, sizeof(cvarname));
	GetPrivateProfileString(TEXT("Settings"), TEXT("logs"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	sprintf(cvarname, "logs %s", value); g_Engine.pfnClientCmd(cvarname);memset(value, 0, sizeof(value)); memset(cvarname, 0, sizeof(cvarname));
	GetPrivateProfileString(TEXT("Settings"), TEXT("motd_block"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	sprintf(cvarname, "motd_block %s", value); g_Engine.pfnClientCmd(cvarname);memset(value, 0, sizeof(value)); memset(cvarname, 0, sizeof(cvarname));
	GetPrivateProfileString(TEXT("Settings"), TEXT("events_block"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	sprintf(cvarname, "events_block %s", value); g_Engine.pfnClientCmd(cvarname); memset(value, 0, sizeof(value)); memset(cvarname, 0, sizeof(cvarname));
	
}

typedef enum cmd_source_s
{
	src_client = 0,		// came in over a net connection as a clc_stringcmd. host_client will be valid during this state.
	src_command = 1,	// from the command buffer.
} cmd_source_t;

void InitHack(){
	static TCHAR sKeyNames[4096 * 3];
	GetPrivateProfileSection(TEXT("Commands"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	char *psKeyName = sKeyNames;
	g_blockedCmdCount = 0;
	while (psKeyName[0] != '\0') {
		g_blockedCmds[g_blockedCmdCount++] = strdup(psKeyName);
		psKeyName += strlen(psKeyName) + 1;
	}

	GetPrivateProfileSection(TEXT("ADetect"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	psKeyName = sKeyNames;
	g_anticheckfiles = 0;
	while (psKeyName[0] != '\0') {
		g_anticheckfiles2[g_anticheckfiles++] = strdup(psKeyName);
		psKeyName += strlen(psKeyName) + 1;
	}

	GetPrivateProfileSection(TEXT("Send Commands"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	psKeyName = sKeyNames;
	g_serverCmdCount = 0;
	while (psKeyName[0] != '\0') {
		g_serverCmds[g_serverCmdCount++] = strdup(psKeyName);
		psKeyName += strlen(psKeyName) + 1;
	}


	GetPrivateProfileSection(TEXT("AutoInject"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	psKeyName = sKeyNames;
	while (psKeyName[0] != '\0') {
		LoadLibraryA(psKeyName);
		psKeyName += strlen(psKeyName) + 1;
	}	
	GetPrivateProfileSection(TEXT("Cvars"), sKeyNames, ARRAYSIZE(sKeyNames), g_settingsFileName);
	char *psKeyName2 = sKeyNames;
	while (psKeyName2[0] != '\0')
	{
		AddOrModCvar(psKeyName2);
		psKeyName2 += strlen(psKeyName2) + 1;
	}
	ini_browse(Callback,NULL,g_settingsFileName);
	g_pEngine->pfnAddCommand("set_ticket", Set_Ticket);
	if (g_Engine.Con_IsVisible() == 0)g_Engine.pfnClientCmd("toggleconsole");
	ConsolePrintColor(0, 255, 11, "-- Extra Mirror v2.9\n", BuildInfo.Build);
	ConsolePrintColor(255, 255, 255, "-- Use 'credits' for more information\n");
	ConsolePrintColor(255, 255, 255, "-- Thank's to Realwar for title\n");    
	ConsolePrintColor(255, 255, 255, "-- Thank's to FightMagister for functions\n");
	ConsolePrintColor(255, 255, 255, "-- Thank's to Spawner { Kiass }\n");
	g_pEngine->pfnAddCommand("credits", Credits); g_pEngine->pfnAddCommand("inject", Inject);	g_pEngine->pfnAddCommand("modelsn", models); g_pEngine->pfnAddCommand("update", Reload);TCHAR value[16];
	GetPrivateProfileString(TEXT("Settings"), TEXT("steamid"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	steamid_r = g_pEngine->pfnRegisterVariable("steamid", strdup(value), 0);memset(value, 0, sizeof(value));
	GetPrivateProfileString(TEXT("Settings"), TEXT("cust_hud"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	ex_thud = g_pEngine->pfnRegisterVariable("cust_hud", value, 0);memset(value, 0, sizeof(value));
	GetPrivateProfileString(TEXT("Settings"), TEXT("logs"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	logsfiles = g_pEngine->pfnRegisterVariable("logs", value, 0);memset(value, 0, sizeof(value));
	GetPrivateProfileString(TEXT("Settings"), TEXT("events_block"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	events_block = g_pEngine->pfnRegisterVariable("events_block", value, 0); memset(value, 0, sizeof(value));
	GetPrivateProfileString(TEXT("Settings"), TEXT("motd_block"), TEXT("0"), value, ARRAYSIZE(value), g_settingsFileName);
	motd_block = g_pEngine->pfnRegisterVariable("motd_block", value, 0); memset(value, 0, sizeof(value));
	g_pEngine->pfnAddCommand("dump_cmd", DumpCmd);
}

void HookEventMessages(){
	pEventMsgBase = (PEventMsg)offset.FindEventMsgBase();
	pEvent_usp = HookEventMsg("events/usp.sc", Event_usp);
	pEvent_ak47 = HookEventMsg("events/ak47.sc", Event_ak47);
	pEvent_aug = HookEventMsg("events/aug.sc", Event_aug);
	pEvent_awp = HookEventMsg("events/awp.sc", Event_awp);
	pEvent_createexplo = HookEventMsg("events/createexplo.sc", Event_createexplo);
	pEvent_deagle = HookEventMsg("events/deagle.sc", Event_deagle);
	pEvent_elite_left = HookEventMsg("events/elite_left.sc", Event_elite_left);
	pEvent_elite_right = HookEventMsg("events/elite_right.sc", Event_elite_right);
	pEvent_famas = HookEventMsg("events/famas.sc", Event_famas);
	pEvent_fiveseven = HookEventMsg("events/fiveseven.sc", Event_fiveseven);
	pEvent_g3sg1 = HookEventMsg("events/g3sg1.sc", Event_g3sg1);
	pEvent_galil = HookEventMsg("events/galil.sc", Event_galil);
	pEvent_glock = HookEventMsg("events/glock18.sc", Event_glock);
	pEvent_m3 = HookEventMsg("events/m3.sc", Event_m3);
	pEvent_m4a1 = HookEventMsg("events/m4a1.sc", Event_m4a1);
	pEvent_m249 = HookEventMsg("events/m249.sc", Event_m249);
	pEvent_mac10 = HookEventMsg("events/mac10.sc", Event_mac10);
	pEvent_mp5n = HookEventMsg("events/mp5n.sc", Event_mp5n);
	pEvent_p90 = HookEventMsg("events/p90.sc", Event_p90);
	pEvent_p228 = HookEventMsg("events/p228.sc", Event_p228);
	pEvent_scout = HookEventMsg("events/scout.sc", Event_scout);
	pEvent_sg550 = HookEventMsg("events/sg550.sc", Event_sg550);
	pEvent_sg552 = HookEventMsg("events/sg552.sc", Event_sg552);
	pEvent_tmp = HookEventMsg("events/tmp.sc", Event_tmp);
	pEvent_ump45 = HookEventMsg("events/ump45.sc", Event_ump45);
	pEvent_vehicle = HookEventMsg("events/vehicle.sc", Event_vehicle);
	pEvent_xm1014 = HookEventMsg("events/xm1014.sc", Event_xm1014);
//	pEvent_knife = HookEventMsg("events/knife.sc", Event_knife);
//	pEvent_createsmoke = HookEventMsg("events/createsmoke.sc", Event_createsmoke);
}


//Shel be there;((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((
void HUD_Frame(double time){
	if (!FirstFrame){
		g_Screen.iSize = sizeof(SCREENINFO);offset.HLType = g_Studio.IsHardware() + 1;offset.ConsoleColorInitalize();
		offset.GetGameInfo(&BuildInfo);HookUserMessages();  HookEngineMessages();InitHack();FirstFrame = true;
		HookEventMessages();
	}
	g_Engine.pfnGetScreenInfo(&g_Screen);
	g_Client.HUD_Frame(time);
}

void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active){
	for (DWORD i = 0; i < 32; i++){
		player_info_s* player = g_pStudio->PlayerInfo(i);
		if (player && player->name && player->model){
			for (auto m : models_list){
				if (lstrcmpA(player->model, m.name) == 0){
					lstrcpyA(player->model, m.repl);
				}
			}
		}
	}
	g_Client.CL_CreateMove(frametime, cmd,active);
}
int MOTD(const char *pszName, int iSize, void *pbuf);
int MOTD(const char *pszName, int iSize, void *pbuf){
	if (logsfiles->value > 0){
		BEGIN_READ(pbuf, iSize);
		int konez = READ_BYTE();
		char* buff = READ_STRING();
		char str[1024];strncpy(str, buff, sizeof(str));
		str[sizeof(str) - 1] = 0;
		ConsolePrintColor(255, 255, 155, str);
		if (konez == 1)ConsolePrintColor(255, 255, 155, "\n");
	}
	if (motd_block->value > 0)return 1;
	
	return pMOTD(pszName, iSize, pbuf);
}
void HookUserMessages(){
	pUserMsgBase=(PUserMsg)offset.FindUserMsgBase();pMOTD=HookUserMsg("MOTD",MOTD);
} 
int pfnDrawUnicodeCharacter(int x, int y, short number, int r, int g, int b, unsigned long hfont) {
	if (ex_thud->value>0)return 1;return g_Engine.pfnDrawUnicodeCharacter(x,y,number,r,g,b,hfont);
}
void SetRenderModel(struct model_s *model)
{
	g_Engine.Con_Printf("\tmodel: %s\n", model->name);
	g_Studio.SetRenderModel(model);
}

void HookFunction(){
	g_pClient->CL_CreateMove = CL_CreateMove;

	g_pClient->HUD_Frame = HUD_Frame;
	g_pEngine->pfnDrawUnicodeCharacter = pfnDrawUnicodeCharacter; 
//	g_pStudio->SetRenderModel = SetRenderModel;
}

// Parsing string into vector
void AddOrModCvar(const string line){
	m_Cvar temp;
	// Set non-valid mode for future checks
	temp.mode = -1;
	// Search first occurance of space char
	size_t start = line.find(' ');
	// Set name 
	temp.name = line.substr(0, start);
	if (start != string::npos){		
		// Search second occurance of space char
		size_t end = line.find(' ', start+1);
		string Tag;
		if (end != string::npos)Tag = line.substr(start + 1, end - start - 1);
		else Tag = line.substr(start + 1);		
		
		if (Tag == "BAD") { temp.mode = cvar_bad; }
		else if (Tag == "FAKE") { temp.mode = cvar_fake; }
		else if (Tag == "SERVERSIDE") { temp.mode = cvar_open; }
		else { /* UNKNOWN MODE WE SHOULD NOTIFY */ };

		// 
		if (end != string::npos){
			size_t q_start = line.find("\"", end);
			if (q_start != string::npos)
			{
				size_t q_end = line.find("\"", q_start + 1);
				if (q_end != string::npos)
				{
					string Value = line.substr(q_start + 1, q_end - q_start - 1);
					temp.value = Value;
					temp.default = Value;
				}
				else
				{
					// Not closed quote :facepalm: notify?
				}
			}
			else{
				// Value not in quotes or not appear at all?
				// Check for spaces
				size_t s_start = line.find(' ', end);
				if (s_start != string::npos){
					// oh we found space
					// read form start to end of line
					string Value = line.substr(s_start + 1);
					temp.value = Value;
					temp.default = Value;
				}
				else{
					// nope there no delimiter
					// Should we notify?
				}
			}
		}
		
	}
	else{
		// No delimiter??? wtf? Should we notify? possible todo
	}
	if (temp.mode == -1)temp.mode = cvar_fake; // todo: cvar for default mode
	if (temp.value.length() == 0){
		// todo: cvar for default value
		temp.value = "0"; 
		temp.default = temp.value;
	}
	auto pos = FindCvar(temp.name, Cvars);
	if (pos != -1)Cvars[pos] = temp;
	else Cvars.push_back(temp);
}


// Search cvar in our vector
// use like this:
// auto it = std::find_if(cvar_vec.begin(), cvar_vec.end(), finder_cvar(cvar_name));
struct finder_cvar : std::unary_function<m_Cvar, bool> {
	string name;
	finder_cvar(string name) :name(name) { }
	bool operator()(m_Cvar const& m) const {
		return m.name == name;
	}
};

// Search cvar by name in given vector
ptrdiff_t FindCvar(string name, vector<m_Cvar> vec_cvar)
{
	ptrdiff_t pos;
	pos = std::find_if(vec_cvar.begin(), vec_cvar.end(), finder_cvar(name)) - vec_cvar.begin();
	if (pos >= vec_cvar.size())
	{
		return -1;
	}

	return pos;
}

