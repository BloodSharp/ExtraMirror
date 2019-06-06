#include "main.h"

#pragma warning(disable:4996)
int* MSG_ReadCount = nullptr;
#define equali !stricmp
int* MSG_CurrentSize = nullptr;
int* MSG_BadRead = nullptr;
int MSG_SavedReadCount = 0;
sizebuf_t* MSG_Buffer = nullptr;
#define MAX_CMD_LINE 2048

extern int g_blockedCmdCount;
extern char *g_blockedCmds[MAX_CMD_LINE];

extern int g_serverCmdCount;
extern char *g_serverCmds[MAX_CMD_LINE];
char com_token[1024];
extern cvar_t *logsfiles;
HL_MSG_ReadByte MSG_ReadByte = nullptr;
HL_MSG_ReadShort MSG_ReadShort = nullptr;
HL_MSG_ReadLong MSG_ReadLong = nullptr;
HL_MSG_ReadFloat MSG_ReadFloat = nullptr;
HL_MSG_ReadString MSG_ReadString = nullptr;
HL_MSG_ReadCoord MSG_ReadCoord = nullptr;
HL_MSG_ReadBitVec3Coord MSG_ReadBitVec3Coord = nullptr;
HL_MSG_ReadBits MSG_ReadBits = nullptr;
HL_MSG_StartBitReading MSG_StartBitReading = nullptr;
HL_MSG_EndBitReading MSG_EndBitReading = nullptr;
void MSG_SaveReadCount() {
	MSG_SavedReadCount = *MSG_ReadCount;
}

void MSG_RestoreReadCount() {
	*MSG_ReadCount = MSG_SavedReadCount;
}
pfnEngineMessage pSVC_VoiceInit;

pfnEngineMessage pSVC_StuffText;
pfnEngineMessage pSVC_SendCvarValue;
pfnEngineMessage pSVC_SendCvarValue2;
pfnEngineMessage pSVC_Director;

void(*Cbuf_Execute)();
void(*Cbuf_AddText)(char *text);

typedef enum cmd_source_s
{
	src_client = 0,		// came in over a net connection as a clc_stringcmd. host_client will be valid during this state.
	src_command = 1,	// from the command buffer.
} cmd_source_t;

void __cdecl Cmd_ExecuteString(char *text, cmd_source_t src);

HOOKINIT(
	ExecuteString_F,								// the type created 
	Cmd_ExecuteString,								// the function prototyped
	ExecuteString_Tramp,							// the trampoline to the original function
	ExecuteString_Prologue						// the prologue object of the function used for this hook
)

DWORD ExecuteString_call;
DWORD ExecuteString_jump;

EasyHook::Hook32 hooker; // an object meant to service you

bool ParseList(const char *str) {
	for (DWORD i = 0; i < g_blockedCmdCount; i++) {
		if (!stricmp(str, g_blockedCmds[i])) {
			return true;
		}
	}
	return false;
}//:D more shit code, in reborn no 

bool ParseList2(const char *str) {
	for (DWORD i = 0; i < g_serverCmdCount; i++) {
		if (!stricmp(str, g_serverCmds[i])) {
			return true;
		}
	}
	return false;
}

int ParseListCvar(const char *str) {
	auto found = FindCvar(str, Cvars);
	if (found == -1)return -1;
	else return Cvars[found].mode;
}

bool IsCommandGood(const char *str) {
	char *ret = g_Engine.COM_ParseFile((char *)str, com_token);
	if (ret == NULL || com_token[0] == 0)return true;
	if ((ParseList(com_token)))return false;
	return true;
}

bool IsCommandGood2(const char *str) {
	char *ret = g_Engine.COM_ParseFile((char *)str, com_token);
	if (ret == NULL || com_token[0] == 0)return true;
	if ((ParseList2(com_token)))return false;
	return true;
}

bool CheckExecute(char *text)
{
	bool isGood = IsCommandGood(text);
	bool isGood2 = IsCommandGood2(text);
	bool isSet = CheckAndSetCvar(text);
	bool isFake = CheckIsFake(text);
	char *x = text;
	if (!isGood2) {
		g_Engine.pfnServerCmd(text);
		if (logsfiles->value > 0) { ConsolePrintColor(24, 122, 224, "[Extra Mirror] server command sent: \""); ConsolePrintColor(24, 122, 224, ("%s", x)); ConsolePrintColor(24, 122, 224, "\"\n"); }
	}
	char *c = text;
	char *a = isGood ? "[Extra Mirror] execute: \"" : "[Extra Mirror] blocked: \"";
	if (logsfiles->value > 0) { ConsolePrintColor(255, 255, 255, ("%s", a)); ConsolePrintColor(255, 255, 255, ("%s", c)); ConsolePrintColor(255, 255, 255, "\"\n"); }
	if (isSet)a = "[Extra Mirror] update server-side cvar: \"";
	if (isSet) { if (logsfiles->value > 0) { ConsolePrintColor(255, 255, 255, ("%s", a)); ConsolePrintColor(255, 255, 255, ("%s", c)); ConsolePrintColor(255, 255, 255, "\"\n"); } }
	if (isGood)return true;
	return false;
}

__declspec(naked) void Cmd_ExecuteString_CallHook( )
{	
	static char *text;
	__asm mov text, ecx
	bool CheckValid;
	CheckValid = CheckExecute(text);
	if (CheckValid)
	{
		__asm {
			 push ebp
			 mov ebp, esp
			 mov ecx, [ebp + 0x8]
			 mov eax, [ebp + 0xC]
			 jmp[ExecuteString_jump]
		}
	}
	else
	{
		__asm ret;
	}
	
}

void ExecuteString_Add(const char *str) {
	ExecuteString_Tramp = (ExecuteString_F)hooker.hook(
		(LPVOID)ExecuteString_call,							// pointer to the function you'd like to hook
		ExecuteString_Prologue,								// the prologue created by the INIT macro
		Cmd_ExecuteString_CallHook							// the hook function to which you want to redirect the original
	);
	Cbuf_AddText((char*)str);
	Cbuf_Execute();
	hooker.unhook(ExecuteString_Tramp, ExecuteString_Prologue);
}

void SVC_SendCvarValue() {
	MSG_SaveReadCount();
	char* cvar = MSG_ReadString();
	char str[1024];
	strncpy(str, cvar, sizeof(str));
	str[sizeof(str) - 1] = 0;
	cvar_t *pCvar = g_Engine.pfnGetCvarPointer(str);
	if (pCvar != NULL) {
		int mode = ParseListCvar(str);
		if (mode == cvar_fake || mode == cvar_open) {
			if (logsfiles->value > 0) {
				ConsolePrintColor(255, 255, 255, "[Extra Mirror] request %s cvar: ", mode == cvar_fake ? "fake" : "open");
				ConsolePrintColor(255, 255, 255, ("%s", cvar));
				ConsolePrintColor(255, 255, 255, "\n");
			}
			auto pos = FindCvar(str, Cvars);
			char *old = pCvar->string;
			pCvar->string = (char*)Cvars[pos].value.c_str();
			MSG_RestoreReadCount();
			pSVC_SendCvarValue();
			pCvar->string = old;
		}
		else if (mode == cvar_bad) {
			if (logsfiles->value > 0) {
				ConsolePrintColor(255, 255, 255, "[Extra Mirror] request blocked cvar: ");
				ConsolePrintColor(255, 255, 255, ("%s", cvar));
				ConsolePrintColor(255, 255, 255, "\n");
			}
			char *old = pCvar->string;
			pCvar->string = "Bad CVAR request";
			MSG_RestoreReadCount();
			pSVC_SendCvarValue();
			pCvar->string = old;
		}
		else {
			if (logsfiles->value > 0) {
				ConsolePrintColor(255, 255, 255, "[Extra Mirror] request cvar: ");
				ConsolePrintColor(255, 255, 255, ("%s", cvar));
				ConsolePrintColor(255, 255, 255, "\n");
			}
			MSG_RestoreReadCount();
			pSVC_SendCvarValue();
		}
	}
	else {
		if (logsfiles->value > 0) {
			ConsolePrintColor(255, 255, 255, "[Extra Mirror] request non-exist cvar: ");
			ConsolePrintColor(255, 255, 255, (" %s", cvar));
			ConsolePrintColor(255, 255, 255, "\n");
		}
		MSG_RestoreReadCount();
		pSVC_SendCvarValue();
	}
}
void SVC_SendCvarValue2() {
	MSG_SaveReadCount();
	MSG_ReadLong();
	char* cvar = MSG_ReadString();
	char str[1024];
	strncpy(str, cvar, sizeof(str));
	str[sizeof(str) - 1] = 0;
	cvar_t *pCvar = g_Engine.pfnGetCvarPointer(str);
	if (pCvar != NULL) {
		int mode = ParseListCvar(str);
		if (mode == cvar_fake || mode == cvar_open) {
			if (logsfiles->value > 0) {
				ConsolePrintColor(255, 255, 255, "[Extra Mirror] request %s cvar2: ", mode == cvar_fake ? "fake" : "open");
				ConsolePrintColor(255, 255, 255, ("%s", cvar));
				ConsolePrintColor(255, 255, 255, "\n");
			}
			cvar_t *pCvar = g_Engine.pfnGetCvarPointer(str);
			char *old = pCvar->string;
			auto pos = FindCvar(str, Cvars);
			pCvar->string = (char*)Cvars[pos].value.c_str();
			MSG_RestoreReadCount();
			pSVC_SendCvarValue2();
			pCvar->string = old;
		}
		else if (mode == cvar_bad) {
			if (logsfiles->value > 0) {
				ConsolePrintColor(255, 255, 255, "[Extra Mirror] request blocked cvar2: ");
				ConsolePrintColor(255, 255, 255, ("%s", cvar));
				ConsolePrintColor(255, 255, 255, "\n");
			}
			cvar_t *pCvar = g_Engine.pfnGetCvarPointer(str);
			char *old = pCvar->string;
			pCvar->string = "Bad CVAR request";
			MSG_RestoreReadCount();
			pSVC_SendCvarValue2();
			pCvar->string = old;
		}
		else {
			if (logsfiles->value > 0) {
				ConsolePrintColor(255, 255, 255, "[Extra Mirror] request cvar2: ");
				ConsolePrintColor(255, 255, 255, ("%s", cvar));
				ConsolePrintColor(255, 255, 255, "\n");
			}
			MSG_RestoreReadCount();
			pSVC_SendCvarValue2();
		}
	}
	else {
		if (logsfiles->value > 0) {
			ConsolePrintColor(255, 255, 255, "[Extra Mirror] request non-exist cvar2: ");
			ConsolePrintColor(255, 255, 255, (" %s", cvar));
			ConsolePrintColor(255, 255, 255, "\n");
		}
		MSG_RestoreReadCount();
		pSVC_SendCvarValue2();
	}
}
bool CheckIsFake(string FullCmd) {
	// Find first space character
	size_t p = FullCmd.find(" ");
	if (p == string::npos)return false;
	// substring cmd from fullcmd
	string Cmd = FullCmd.substr(0, p);
	auto pos = FindCvar(Cmd, Cvars);
	if (pos == -1)return false;
	if (Cvars[pos].mode == cvar_fake)return true;
	return false;
}

bool CheckAndSetCvar(string FullCmd) {
	// Find first space character
	size_t p = FullCmd.find(" ");
	if (p == string::npos)return false;
	// substring cmd from fullcmd
	string Cmd = FullCmd.substr(0, p);
	auto pos = FindCvar(Cmd, Cvars);
	if (pos == -1)return false;
	if (Cvars[pos].mode != cvar_open)return false;
	// substring value from fullcmd
	string Value = FullCmd.substr(p + 1);
	Cvars[pos].value = Value;
	return true;
}
void SVC_StuffText() {
	char* command = MSG_ReadString();
	ExecuteString_Add(command);
}
void SVC_Director() {
	MSG_SaveReadCount();
	int msglen = MSG_ReadByte();
	int msgtype = MSG_ReadByte();
	if (msgtype == 10) {
		char* command = MSG_ReadString();
		ExecuteString_Add(command);
	}
	else
	{
		MSG_RestoreReadCount();
		pSVC_Director();
	}
}

void SVC_VoiceInit() {
	MSG_SaveReadCount();
	char* codec = MSG_ReadString(); int bitz = MSG_ReadByte(); bool blocked;
	if (!stricmp(codec, "voice_miles") || !stricmp(codec, "voice_speex"))blocked = false;
	else blocked = true;
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "[Extra Mirror] [VoiceInit] %s [%s]\n", codec, blocked ? "Blocked" : "Execute");
	ConsolePrintColor(255, 255, 255, buffer);
	if (blocked)return;
	MSG_RestoreReadCount();
	pSVC_VoiceInit();
}