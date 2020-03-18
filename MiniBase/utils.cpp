#include "utils.h"

char* native_strcpy(char* dest, const char* src)
{
	return lstrcpyA(dest, src);
}

char* native_strcat(char* dest, const char* src)
{
	return lstrcatA(dest, src);
}

int native_strcmp(char* pStr1, char* pStr2)
{
	return lstrcmpA(pStr1, pStr2);
}

int native_strlen(char* pStr)
{
	return lstrlenA(pStr);
}

void native_memcpy(void* dst, const void* src, size_t count)
{
	_asm
	{
		mov	edi, [dst]
		mov	esi, [src]
		mov	ecx, [count]
		rep	movsb
	}
}

void native_memset(void* szBuffer, DWORD dwLen, DWORD dwSym)
{
	_asm
	{
		pushad
		mov edi, [szBuffer]
		mov ecx, [dwLen]
		mov eax, [dwSym]
		rep stosb
		popad
	}
}


PEngineMsg EngineMsgByName(char* szMsgName)
{
	PEngineMsg Ptr = nullptr;
	Ptr = pEngineMsgBase;

	while (native_strcmp(Ptr->name, OFF_SVC_MESSAGES_P))
	{
		if (!native_strcmp(Ptr->name, szMsgName))
			return Ptr;

		Ptr++;
	}

	Ptr->pfn = 0;
	return Ptr;
}

pfnEngineMessage HookEngineMsg(char* szMsgName, pfnEngineMessage pfn)
{
	PEngineMsg Ptr = nullptr;
	pfnEngineMessage Original = nullptr;

	Ptr = EngineMsgByName(szMsgName);

	if (Ptr->pfn != 0)
	{
		Original = Ptr->pfn;
		Ptr->pfn = pfn;
	}
	else
	{
		MessageBoxA(0, szMsgName, 0, MB_OK | MB_ICONERROR);
	}

	return Original;
}
