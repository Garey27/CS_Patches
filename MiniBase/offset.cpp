#include "offset.h"

cOffset offset;

void cOffset::GetRenderType()
{
	HwDll = (DWORD)GetModuleHandleA(HW_DLL);
	SwDll = (DWORD)GetModuleHandleA(SW_DLL);
	HlMod = (DWORD)GetModuleHandleA(NULL);
}

bool cOffset::GetModuleInfo()
{
	GetRenderType();

	if (HwDll)
		HwBase = HwDll;
	else if (SwDll)
		HwBase = SwDll;
	else
		HwBase = HlMod;

	HwSize = GetModuleSize(HwBase);
	HwEnd = HwBase + HwSize - 1;

	HlBase = HlMod;
	HlSize = (DWORD)GetModuleSize(HlBase);
	HlEnd = HlBase + HlSize - 1;

	ClBase = (DWORD)GetModuleHandleA(CLIENT_DLL);

	if (ClBase)
	{
		ClSize = GetModuleSize(ClBase);
		ClEnd = ClBase + ClSize - 1;
	}
	else
	{
		ClBase = HwBase;
		ClEnd = HwEnd;
		ClSize = HwSize;
	}

	VgBase = (DWORD)GetModuleHandleA(GAMEUI_DLL);

	if (VgBase)
	{
		VgSize = (DWORD)GetModuleSize(VgBase);
		VgEnd = VgBase + VgSize - 1;
	}

	return (HwBase && ClBase && HlBase && VgBase);
}

void cOffset::Error(char* Msg)
{
	MessageBoxA(0, Msg, OFF_ERROR, MB_OK | MB_ICONERROR);
}

DWORD cOffset::FindClientTable()
{
	BYTE ClientOffset[2] = { 0x10, 0x13 };

	DWORD PatternAddress = FindPattern(OFF_CLIENT_PATTERN, HwBase, HwEnd, 0);

	if (PatternAddress)
	{
		for (byte i = 0; i < sizeof(ClientOffset); i++)
		{
			DWORD ClientTablePtr = *(PDWORD)(FindReference(HwBase, HwEnd, PatternAddress) + ClientOffset[i]);

			if (!FarProc((DWORD)ClientTablePtr, HwBase, HwEnd) &&
				!IsBadReadPtr((PVOID)ClientTablePtr, sizeof(cl_clientfunc_t)))
			{
				return ClientTablePtr;
			}
		}
	}

	return 0;
}

DWORD cOffset::FindEngineTable()
{
	DWORD PatternAddress = FindPattern(OFF_ENGINE_PATTERN, OFF_ENGINE_MASK, ClBase, ClEnd, 0x02);

	if (PatternAddress)
	{
		if (!FarProc((DWORD)PatternAddress, ClBase, ClEnd))
		{
			return *(PDWORD)PatternAddress;
		}
	}
	else
	{
		PatternAddress = FindPattern(OFF_ENGINE_PATTERN, OFF_ENGINE_MASK, HlBase, HlEnd, 0x02);

		if (PatternAddress)
		{
			if (!FarProc((DWORD)PatternAddress, HlBase, HlEnd))
			{
				return *(PDWORD)PatternAddress;
			}
		}
	}

	return 0;
}

DWORD cOffset::FindStudioTable()
{
	DWORD StudioTablePtr = *(DWORD*)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x30); // old patch, dod

	if (FarProc((DWORD)StudioTablePtr, HwBase, HwEnd) && FarProc((DWORD)StudioTablePtr, HlBase, HlEnd) &&
		FarProc((DWORD)StudioTablePtr, ClBase, ClEnd))
	{
		StudioTablePtr = *(DWORD*)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x1A); // new patch / steam	

		if (FarProc((DWORD)StudioTablePtr, ClBase, ClEnd))
			return 0;
	}

	return StudioTablePtr;
}

DWORD cOffset::FindUserMsgBase()
{
	BYTE Pattern_UserMsg[9] =
	{
		0x52, 0x50, 0xE8, 0xFF, 0xFF, 0xFF, 0xFF, 0x83, 0x00
	};

	BYTE Pattern_UserMsg2[13] =
	{
		0xFF, 0xFF, 0xFF, 0x0C,
		0x56, 0x8B, 0x35, 0xFF, 0xFF, 0xFF, 0xFF, 0x57, 0x00
	};

	DWORD Address = (DWORD)g_Engine.pfnHookUserMsg;
	DWORD UserMsgBase = Absolute(FindPattern((PCHAR)Pattern_UserMsg, OFF_MSG_USER_MASK1, Address, Address + 0x32, 3));

	if (FarProc(UserMsgBase, HwBase, HwEnd))
	{
		Error(OFF_USER_MSG_EROR1);
		return 0;
	}

	UserMsgBase = FindPattern((PCHAR)Pattern_UserMsg2, OFF_MSG_USER_MASK2, UserMsgBase, UserMsgBase + 0x32, 7);

	if (FarProc(UserMsgBase, HwBase, HwEnd))
	{
		Error(OFF_USER_MSG_EROR2);
		return 0;
	}

	return **(PDWORD*)UserMsgBase;
}

DWORD cOffset::FindGameConsole()
{
	DWORD PatternAddress = FindPattern(OFF_GAME_CONSOLE_P, VgBase, VgEnd, 0);
	DWORD ReferenAddress = FindReference(VgBase, VgEnd, PatternAddress) + 0x21;

	if (FarProc(ReferenAddress, VgBase, VgEnd))
	{
		Error(OFF_GAME_CONSOLE_R);
		return 0;
	}

	DWORD GameConsole = *(PDWORD)ReferenAddress;

	return GameConsole;
}

#define equali !stricmp
void cOffset::ConsoleColorInitalize()
{
	DWORD GameConsole = FindGameConsole();

	if (GameConsole)
	{
		DWORD Panel = (*(PDWORD)(GameConsole + 8) - GameConsole);

		Console_TextColor = PColor24(Panel + GameConsole + 288 + sizeof(DWORD));

		if (*(PDWORD)(DWORD(Console_TextColor) + 8) != 0)
		{
			Console_TextColor = PColor24(Panel + GameConsole + 288 + (sizeof(DWORD) * 2));
		}
	}
}

void cOffset::CopyClient()
{
	native_memcpy(&g_Client, g_pClient, sizeof(cl_clientfunc_t));
}

void cOffset::CopyEngine()
{
	native_memcpy(&g_Engine, g_pEngine, sizeof(cl_enginefunc_t));
}

void cOffset::CopyStudio()
{
	native_memcpy(&g_Studio, g_pStudio, sizeof(engine_studio_api_t));
}

DWORD cOffset::Absolute(DWORD Address)
{
	return Address + *(PDWORD)Address + 4;
}

DWORD cOffset::FarProc(DWORD Address, DWORD LB, DWORD HB)
{
	return ((Address < LB) || (Address > HB));
}

DWORD cOffset::FindReference(DWORD start, DWORD end, DWORD Address)
{
	char szPattern[] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0x00 };
	*(PDWORD)&szPattern[1] = Address;
	return FindPattern(szPattern, start, end, 0);
}

DWORD cOffset::FindPattern(PCHAR pattern, PCHAR mask, DWORD start, DWORD end, DWORD offset)
{
	int patternLength = native_strlen(pattern);
	bool found = false;

	for (DWORD i = start; i < end - patternLength; i++)
	{
		found = true;
		for (int idx = 0; idx < patternLength; idx++)
		{
			if (mask[idx] == 'x' && pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return i + offset;
		}
	}

	return 0;
}

DWORD cOffset::FindPattern(PCHAR pattern, DWORD start, DWORD end, DWORD offset)
{
	int patternLength = native_strlen(pattern);
	bool found = false;

	for (DWORD i = start; i < end - patternLength; i++)
	{
		found = true;
		for (int idx = 0; idx < patternLength; idx++)
		{
			if (pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return i + offset;
		}
	}

	return 0;
}

DWORD cOffset::GetModuleSize(DWORD Address)
{
	return PIMAGE_NT_HEADERS(Address + (DWORD)PIMAGE_DOS_HEADER(Address)->e_lfanew)->OptionalHeader.SizeOfImage;
}