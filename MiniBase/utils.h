#pragma once

#include "main.h"

#define HW_DLL				"hw.dll"
#define SW_DLL				"sw.dll"

#define CLIENT_DLL			"client.dll"
#define GAMEUI_DLL			"GameUI.dll"

#define ERROR_FIND			"Find Cl/En/St Modules Error"
#define OFF_ERROR			"Error Find"

#define OFF_CLIENT_PATTERN	"ScreenFade"

#define OFF_ENGINE_PATTERN	"\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF"
#define OFF_ENGINE_MASK		"xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????"
#define OFF_EVENT_MSG_BASE	"CL_HookEvent:  Must provide a valid event name"
#define OFF_STUDIO_PATTERN	"Couldn't get client .dll studio model rendering interface."

#define OFF_SVC_MSG_PATTERN "\xBF\xFF\xFF\xFF\xFF\x8B\x04\xB5\xFF\xFF\xFF\xFF\x85\xC0\x74\xFF\x81\xFF\xFF\xFF\xFF\xFF\x7F\x04\x8B\x0F\xEB\x05"
#define OFF_SVC_MSG_MASK	"x????xxx????xxx?xx????xxxxxx"

#define OFF_MSG_USER_MASK1	"xxx????x"
#define OFF_MSG_USER_MASK2	"???xxxx????x"
#define OFF_EVENT_MSG_ERROR	"Couldn't find EventMsgBase pointer."
#define OFF_USER_MSG_EROR1	"Couldn't find UserMsgBase #1 pointer."
#define OFF_USER_MSG_EROR2	"Couldn't find UserMsgBase #2 pointer."

#define OFF_GAME_CONSOLE_P	"GameConsole003"
#define OFF_GAME_CONSOLE_R	"Couldn't find GameConsole pointer."

#define OFF_MSG_READ_CORD	"MSG_ReadCoord"
#define OFF_MSG_STR_READING	"MSG_StartBitReading"
#define OFF_MSG_END_READING	"MSG_EndBitReading"
#define OFF_ENGINE_MSG_BASE	"EngineMsgBase"
#define OFF_SVC_MESSAGES_P	"End of List"

#define OFF_ERR_GAMEINFO	"Couldn't find GameInfo pointer."

PUserMsg UserMsgByName(char* szMsgName);
PEngineMsg EngineMsgByName(char* szMsgName);
pcmd_t CommandByName(char* szName);

pfnUserMsgHook HookUserMsg(char *szMsgName, pfnUserMsgHook pfn);
pfnEngineMessage HookEngineMsg(char *szMsgName, pfnEngineMessage pfn);
pfnEventMsgHook HookEventMsg(char *szMsgName, pfnEventMsgHook pfn);

PEventMsg EventMsgByName(char* szMsgName);
char* native_strcpy(char *dest, const char *src);
char* native_strcat(char *dest, const char *src);

int native_strcmp(char *pStr1, char *pStr2);
int native_strlen(char *pStr);

void native_memcpy(void * dst, const void * src, size_t count);
void native_memset(void *szBuffer, DWORD dwLen, DWORD dwSym);