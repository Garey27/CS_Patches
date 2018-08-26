#include "main.h"
#include <thread>
#include <string>

cl_clientfunc_t *g_pClient = nullptr;
cl_enginefunc_t *g_pEngine = nullptr;
engine_studio_api_t *g_pStudio = nullptr;

cl_clientfunc_t g_Client;
cl_enginefunc_t g_Engine;
engine_studio_api_t g_Studio;
PColor24 Console_TextColor;

char* BaseDir;
bool FirstFrame = false;

std::vector<uint8_t> HexToBytes(const std::string& hex) {
	std::vector<uint8_t> bytes;

	for (unsigned int i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
		bytes.push_back(byte);
	}

	return bytes;
}

void HexReplaceInLibrary(std::string libraryPath, std::string hexSearch, std::string hexReplace, uint8_t wcard, uint64_t offset) {
	const uint8_t wildcard = wcard;
	auto search_comparator = [&wildcard](uint8_t val1, uint8_t val2)
	{
		return (val1 == val2 || (wildcard && val2 == wildcard));
	};
	auto replace_comparator = [&wildcard](uint8_t val, uint8_t val2)
	{
		if (!wildcard)
			return val;
		else if (val == wildcard)
		{
			return val2;
		}

		return val;
	};

	auto libraryAddress = GetModuleHandleA(libraryPath.c_str());
	auto dosHeader = (IMAGE_DOS_HEADER *)libraryAddress;
	auto peHeader = (IMAGE_NT_HEADERS *)((uintptr_t)libraryAddress + (uintptr_t)dosHeader->e_lfanew);

	std::vector<uint8_t> searchbytes = HexToBytes(hexSearch);
	std::vector<uint8_t> replacebytes = HexToBytes(hexReplace);

	auto searchSize = hexSearch.length() / 2;
	auto codeBase = (uintptr_t)libraryAddress + peHeader->OptionalHeader.BaseOfCode;
	auto codeSize = peHeader->OptionalHeader.SizeOfCode;
	auto codeEnd = codeBase + codeSize;
	auto codeSearchEnd = codeEnd - searchSize + 1;
	auto cstart = (uint8_t*)codeBase;
	for (;;)
	{
		uint8_t* res = std::search(cstart, (uint8_t*)codeSearchEnd, searchbytes.begin(), searchbytes.end(), search_comparator);
		if (res >= (uint8_t*)codeSearchEnd)
		{
			break;
		}
		DWORD oldProt;
		VirtualProtect((LPVOID)res, searchbytes.size(), PAGE_EXECUTE_READWRITE, &oldProt);
		std::transform(replacebytes.begin(), replacebytes.end(), res + offset, res + offset, replace_comparator);
		VirtualProtect((LPVOID)res, searchbytes.size(), oldProt, &oldProt);
		cstart = res + searchbytes.size();
	}
}
extern void SearchPrintConsole();
void ModuleLoaded()
{
	offset.ConsoleColorInitalize();
	SearchPrintConsole();
	FirstFrame = true;
}


void ModuleEntry( )
{
	DWORD ClientTable = NULL, EngineTable = NULL, StudioTable = NULL;
	while (1)
	{
		if (offset.GetModuleInfo())
		{
			if (!ClientTable)
			{
				ClientTable = offset.FindClientTable();
				if (ClientTable)
				{
					g_pClient = (cl_clientfunc_t*)ClientTable;
					offset.CopyClient();
				}
				else continue;
			}

			if (!EngineTable)
			{
				EngineTable = offset.FindEngineTable();
				if (EngineTable)
				{
					g_pEngine = (cl_enginefunc_t*)EngineTable;
					offset.CopyEngine();
				}
				else continue;
			}

			if (!StudioTable)
			{
				StudioTable = offset.FindStudioTable();
				if (StudioTable)
				{
					g_pStudio = (engine_studio_api_t*)StudioTable;
					offset.CopyStudio();
				}
				else continue;
			}

			if (ClientTable && EngineTable && StudioTable)
			{
				ModuleLoaded();
				return;
			}
		}
		std::this_thread::sleep_for(100ms);
	}
}

extern "C" __declspec( dllexport ) BOOL WINAPI RIB_Main ( LPVOID lp, LPVOID lp2, LPVOID lp3, LPVOID lp4, LPVOID lp5 ) 
{
	return TRUE;
}


DWORD WINAPI ThreadEntry(LPVOID lpThreadParameter)
{
	std::thread(ModuleEntry).detach();
}
HINSTANCE hDLL;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved){
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			hDLL = hinstDLL;			
			//unicode patch for console
			HexReplaceInLibrary("cstrike/cl_dlls/client.dll", "7ECC880A", "76CC880A", 0xCC, 0);
			//1280x720<= tab avatar fixes
			HexReplaceInLibrary("cstrike/cl_dlls/client.dll", "817C24CC00030000", "817C24CC01000000", 0xCC, 0);
			HexReplaceInLibrary("cstrike/cl_dlls/client.dll", "3D000300CC8B4424CC", "3D010000CC8B4424CC", 0xCC, 0);
			//wad files download fix			
			HexReplaceInLibrary("hw.dll", "1885C07403C600008D85", "1885C07414C600008D85", 0, 0);
			//GetHullBounds Fix
			HexReplaceInLibrary("hw.dll", "8B4C240433C02BC87406497403497505B801000000C3", "837C240401750B8B44240CC7400800000042B001C3C3", 0, 0);
			CreateThread(0, 0, ThreadEntry, 0, 0, 0);
			
			return TRUE;
		}
	return FALSE;
}