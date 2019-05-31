#include "main.h"
#include "ValveSDK/tier1/UtlVector.h"

#define MAX_LINES 5
#define MAX_CHARS_PER_LINE 256

struct TextRange
{
	int start;
	int end;
	float *color;
};

struct SayTextLine
{
	wchar_t m_line[MAX_CHARS_PER_LINE];
	CUtlVector<TextRange> m_textRanges;
	int m_clientIndex;
	float *m_teamColor;
};

void ConsolePrintColor(BYTE R, BYTE G, BYTE B, const char *fmt, ...) {
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	TColor24 DefaultColor; PColor24 Ptr; Ptr = Console_TextColor; DefaultColor = *Ptr; Ptr->R = R; Ptr->G = G; Ptr->B = B; g_Engine.Con_Printf(buf); *Ptr = DefaultColor;
}

#define g_sayTextLine (*pg_sayTextLine)
SayTextLine g_sayTextLine[MAX_LINES + 1];

std::string ws2s(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

#define F2B(f) ((f) >= 1.0f ? 255 : (int)((f)*256.f))
void ColorChatConsolePrint(char string[512])
{
	int rangeIndex;
	TextRange *range;
	int total_length = 0;
	static auto pCvarColor = g_Engine.pfnGetCvarPointer("con_color");
	char *pszColor = pCvarColor->string;

	BYTE Cvar_R, Cvar_G, Cvar_B;
	BYTE R, G, B;
	sscanf_s(pszColor, "%hhi %hhi %hhi", &Cvar_R, &Cvar_G, &Cvar_B);
	if (g_sayTextLine[0].m_textRanges.Count() != 0)
	{
		for (rangeIndex = 0; rangeIndex < g_sayTextLine[0].m_textRanges.Count(); rangeIndex++)
		{
			range = &g_sayTextLine[0].m_textRanges[rangeIndex];
			if (range->color) {
				R = F2B(range->color[0]);
				G = F2B(range->color[1]);
				B = F2B(range->color[2]);
			}
			else {
				R = Cvar_R; G = Cvar_G; B = Cvar_B;
			}
			ConsolePrintColor(R, G, B, ws2s(wstring(&g_sayTextLine[0].m_line[range->start], range->end - range->start)).c_str());
		}
		// Print newline char if needed
		std::string check_string(&string[0], total_length);
		if (check_string.find("\n") == string::npos)
		{
			g_Engine.pfnConsolePrint("\n");
		}
	}
}
void SearchPrintConsole()
{
	DWORD PatternAddress = offset.FindPattern("\xFF\x15\xFF\xFF\xFF\xFF\x83\xC4\xFF\x66\x89\x1D\xFF\xFF\xFF\xFF", "xx????xx?xxx????", offset.ClBase, offset.ClEnd, 0x1);

	if (PatternAddress)
	{
		pg_sayTextLine = reinterpret_cast<decltype(pg_sayTextLine)>(*(DWORD*)offset.FindPattern("\x66\x39\x1D\xFF\xFF\xFF\xFF\x55", "xxx????x", offset.ClBase, offset.ClEnd, 0x3));

		DWORD oldProt;
		VirtualProtect(LPVOID(PatternAddress - 1), 6, PAGE_EXECUTE_READWRITE, &oldProt);
		*(uint8_t *)(PatternAddress - 1) = 0x90; // NOP
		*(uint8_t *)PatternAddress = 0xe8; // CALL
		*(intptr_t *)(PatternAddress + 1) = (uintptr_t)ColorChatConsolePrint - (PatternAddress + 5); // ADDRESS		
		VirtualProtect(LPVOID(PatternAddress - 1), 6, oldProt, &oldProt);
	}
}