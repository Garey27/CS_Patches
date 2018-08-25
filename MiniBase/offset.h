#pragma once

#include "main.h"

class cOffset
{
public:

	DWORD HwDll , SwDll , HlMod;

	DWORD HwBase , HwSize , HwEnd;
	DWORD ClBase , ClSize , ClEnd;
	DWORD HlBase , HlSize , HlEnd;
	DWORD VgBase , VgSize , VgEnd;

	BYTE HLType;

	void GetRenderType();
	bool GetModuleInfo();

	void Error( char* Msg );

	DWORD FindClientTable();
	DWORD FindEngineTable();
	DWORD FindStudioTable();
	DWORD FindUserMsgBase();
	DWORD FindGameConsole();
	DWORD FindSVCMessages();
	DWORD FindEventMsgBase();
	void ConsoleColorInitalize();
	void GetGameInfo( pGameInfo_s GameInfo );

	void CopyClient();
	void CopyEngine();
	void CopyStudio();

	DWORD Absolute( DWORD Address );

	DWORD FarProc( DWORD Address , DWORD LB , DWORD HB );
	DWORD FindReference( DWORD start , DWORD end , DWORD Address );

	DWORD FindPattern( PCHAR pattern , PCHAR mask , DWORD start , DWORD end , DWORD offset );
	DWORD FindPattern( PCHAR pattern , DWORD start , DWORD end , DWORD offset );

	DWORD GetModuleSize( DWORD Address );
};

extern cOffset offset;