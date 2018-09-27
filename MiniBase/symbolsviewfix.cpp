#include "main.h"

BOOL WINAPI GetCharABCWidthsW_Hooked(void *pThis, HDC hdc, UINT wFirst, UINT wLast, LPABC lpABC)
{
	BOOL bRet = GetCharABCWidthsW(hdc, wFirst, wLast, lpABC);

	lpABC->abcA = lpABC->abcA - *(int *)((BYTE *)pThis + 632);
	lpABC->abcB = lpABC->abcB + *(int *)((BYTE *)pThis + 48) + *(int *)((BYTE *)pThis + 632) * 2;
	lpABC->abcC = lpABC->abcC - *(int *)((BYTE *)pThis + 48) - *(int *)((BYTE *)pThis + 632);

	return bRet;
}

void __declspec(naked) GetCharABCWidthsW_Helper(HDC hdc, UINT wFirst, UINT wLast, LPABC lpABC)
{
	_asm
	{
		push[esp]
		mov[esp + 4], ebp;
		jmp GetCharABCWidthsW_Hooked;
	}
}

void SymbolsViewFix()
{
	BYTE *pCall = (BYTE *)offset.FindPattern("\xFF\x15\xFF\xFF\xFF\xFF\x85\xC0\x74\x20", "xx????xxxx", offset.HwBase, offset.HwEnd, 0);
	
	if (pCall)
	{
		DWORD dwOldProt;
		VirtualProtect(pCall, 6, PAGE_EXECUTE_READWRITE, &dwOldProt);
		*pCall = 0xE8;
		*(LONG *)(pCall + 1) = (LONG)&GetCharABCWidthsW_Helper - ((LONG)pCall + 5);
		*(pCall + 5) = 0x90;
		VirtualProtect(pCall, 6, dwOldProt, &dwOldProt);
	}
}