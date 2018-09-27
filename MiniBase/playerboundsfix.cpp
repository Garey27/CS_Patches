#include "main.h"

void FixPlayerBounds()
{
	DWORD CL_GetPlayerHulls_Address = offset.FindPattern("\x56\x57\x33\xFF\x33\xF6\x8D\x86\xFF\xFF\xFF\xFF\x8D\x8E\xFF\xFF\xFF\xFF\x50\x51\x57\xE8", "xxxxxxxx????xx????xxxx", offset.HwBase, offset.HwEnd, 0);

	if (CL_GetPlayerHulls_Address)
	{
		// player_maxs[1][2] = 32.0;
		((float *)*(DWORD *)(CL_GetPlayerHulls_Address + 0x8))[5] = 32.0;
	}
}