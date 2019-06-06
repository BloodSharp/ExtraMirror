#pragma once

#include "Main.h"

#define BIT(n) (1<<(n))

#define DEFAULT_SOUND_PACKET_VOLUME			255
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0f
#define DEFAULT_SOUND_PACKET_PITCH			100

// Sound flags
#define SND_FL_VOLUME			BIT(0)	// send volume
#define SND_FL_ATTENUATION		BIT(1)	// send attenuation
#define SND_FL_LARGE_INDEX		BIT(2)	// send sound number as short instead of byte
#define SND_FL_PITCH			BIT(3)	// send pitch
#define SND_FL_SENTENCE			BIT(4)	// set if sound num is actually a sentence num
#define SND_FL_STOP				BIT(5)	// stop the sound
#define SND_FL_CHANGE_VOL		BIT(6)	// change sound vol
#define SND_FL_CHANGE_PITCH		BIT(7)	// change sound pitch
#define SND_FL_SPAWNING			BIT(8)	// we're spawning, used in some cases for ambients (not sent across network)

// director command types:
#define DRC_CMD_NONE				0	// NULL director command
#define DRC_CMD_START				1	// start director mode
#define DRC_CMD_EVENT				2	// informs about director command
#define DRC_CMD_MODE				3	// switches camera modes
#define DRC_CMD_CAMERA				4	// set fixed camera
#define DRC_CMD_TIMESCALE			5	// sets time scale
#define DRC_CMD_MESSAGE				6	// send HUD centerprint
#define DRC_CMD_SOUND				7	// plays a particular sound
#define DRC_CMD_STATUS				8	// HLTV broadcast status
#define DRC_CMD_BANNER				9	// set GUI banner
#define DRC_CMD_STUFFTEXT			10	// like the normal svc_stufftext but as director command
#define DRC_CMD_CHASE				11	// chase a certain player
#define DRC_CMD_INEYE				12	// view player through own eyes
#define DRC_CMD_MAP					13	// show overview map
#define DRC_CMD_CAMPATH				14	// define camera waypoint
#define DRC_CMD_WAYPOINTS			15	// start moving camera, inetranl message
#define DRC_CMD_LAST				15

extern int* MSG_ReadCount;
extern int* MSG_CurrentSize;
extern int* MSG_BadRead;
extern int MSG_SavedReadCount;
extern sizebuf_t* MSG_Buffer;

typedef byte(*HL_MSG_ReadByte)();
typedef short(*HL_MSG_ReadShort)();
typedef int(*HL_MSG_ReadLong)();
typedef float(*HL_MSG_ReadFloat)();
typedef char* (*HL_MSG_ReadString)();
typedef float(*HL_MSG_ReadCoord)();

typedef void(*HL_MSG_ReadBitVec3Coord)(float* fa);
typedef int(*HL_MSG_ReadBits)(int Count);
typedef void(*HL_MSG_StartBitReading)(sizebuf_t* buffer);
typedef void(*HL_MSG_EndBitReading)(sizebuf_t* buffer);

extern HL_MSG_ReadByte MSG_ReadByte;
extern HL_MSG_ReadShort MSG_ReadShort;
extern HL_MSG_ReadLong MSG_ReadLong;
extern HL_MSG_ReadFloat MSG_ReadFloat;
extern HL_MSG_ReadString MSG_ReadString;
extern HL_MSG_ReadCoord MSG_ReadCoord;

extern HL_MSG_ReadBitVec3Coord MSG_ReadBitVec3Coord;
extern HL_MSG_ReadBits MSG_ReadBits;
extern HL_MSG_StartBitReading MSG_StartBitReading;
extern HL_MSG_EndBitReading MSG_EndBitReading;
/*
typedef void(*HL_MSG_CBuf_AddText)(char* text);
extern HL_MSG_CBuf_AddText CBuf_AddText_Orign;
*/
void MSG_SaveReadCount();
void MSG_RestoreReadCount();
//void CBuf_AddText(char* text);

void SVC_StuffText();
void SVC_SendCvarValue();
void SVC_SendCvarValue2();
void SVC_Director();
void SVC_Resourcelist();
void SVC_VoiceInit();

extern DWORD ExecuteString_call;
extern DWORD ExecuteString_jump;
extern void(*Cbuf_Execute)();
extern void(*Cbuf_AddText)(char *text);

extern pfnEngineMessage pSVC_VoiceInit;
extern pfnEngineMessage pSVC_StuffText;
extern pfnEngineMessage pSVC_SendCvarValue;
extern pfnEngineMessage pSVC_SendCvarValue2;
extern pfnEngineMessage pSVC_Director;
extern bool CheckIsFake(string FullCmd);
extern bool CheckAndSetCvar(string FullCmd);