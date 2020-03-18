#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#pragma warning(disable:4101)
#pragma warning(disable:4244)
#pragma warning(disable:4800)
#pragma warning(disable:4244)
#pragma warning(disable:4995)
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#include "ValveSDK/engine/wrect.h"
#include "ValveSDK/engine/cl_dll.h"
#include "ValveSDK/engine/r_studioint.h"
#include "ValveSDK/engine/cl_entity.h"
#include "ValveSDK/misc/com_model.h"
#include "ValveSDK/engine/triangleapi.h"
#include "ValveSDK/engine/pmtrace.h"
#include "ValveSDK/engine/pm_defs.h"
#include "ValveSDK/engine/pm_info.h"
#include "ValveSDK/common/ref_params.h"
#include "ValveSDK/common/studio_event.h"
#include "ValveSDK/common/net_api.h"
#include "ValveSDK/common/r_efx.h"
#include "ValveSDK/engine/cvardef.h"
#include "ValveSDK/engine/util_vector.h"
#include "ValveSDK/engine/studio.h"
#include "ValveSDK/engine/event_args.h"
#include "ValveSDK/engine/event_flags.h"
#include "ValveSDK/common/event_api.h"
#include "ValveSDK/common/screenfade.h"
#include "ValveSDK/engine/keydefs.h"
#include "ValveSDK/common/engine_launcher_api.h"
#include "ValveSDK/common/entity_types.h"

#include "mem/patternscan.h"
#include "detours.h"
#include "struct.h"
#include "offset.h"
#include "utils.h"

extern cl_clientfunc_t* g_pClient;
extern cl_enginefunc_t* g_pEngine;
extern engine_studio_api_t* g_pStudio;

extern cl_clientfunc_t g_Client;
extern cl_enginefunc_t g_Engine;
extern engine_studio_api_t g_Studio;

extern PUserMsg pUserMsgBase;
extern PEngineMsg pEngineMsgBase;
extern PEventMsg pEventMsgBase;
extern PColor24 Console_TextColor;

extern SCREENINFO g_Screen;

extern char* BaseDir;


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
typedef int(*HL_MSG_ReadBuf)(int iSize, void* pbuf);

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
extern HL_MSG_ReadBuf MSG_ReadBuf;

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