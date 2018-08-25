#include "utils.h"

char* native_strcpy(char *dest, const char *src)
{
	return lstrcpyA(dest, src);
}

char* native_strcat(char *dest, const char *src)
{
	return lstrcatA(dest, src);
}

int native_strcmp(char *pStr1, char *pStr2)
{
	return lstrcmpA(pStr1, pStr2);
}

int native_strlen(char *pStr)
{
	return lstrlenA(pStr);
}

void native_memcpy(void * dst, const void * src, size_t count)
{
	_asm
	{
		mov	edi, [dst]
			mov	esi, [src]
			mov	ecx, [count]
			rep	movsb
	}
}

void native_memset(void *szBuffer, DWORD dwLen, DWORD dwSym)
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