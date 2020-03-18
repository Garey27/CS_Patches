/*
 * Steam voice enchancer?
 */
#include "main.h"
#include "Windows.h"
#include "steam/isteamuser.h"
#include <cstring>
#include <memory>
#include "VMT_Hook.h"
#include <thread>
#include <chrono>
#include <vector>

#include "ao/ao.h"
#include <mutex>
#include <sstream>
#include <algorithm>
int* MSG_ReadCount = nullptr;
int* MSG_CurrentSize = nullptr;
int* MSG_BadRead = nullptr;
int MSG_SavedReadCount = 0;
sizebuf_t* MSG_Buffer = nullptr;
PEngineMsg pEngineMsgBase;

HL_MSG_ReadByte MSG_ReadByte = nullptr;
HL_MSG_ReadShort MSG_ReadShort = nullptr;
HL_MSG_ReadLong MSG_ReadLong = nullptr;
HL_MSG_ReadFloat MSG_ReadFloat = nullptr;
HL_MSG_ReadString MSG_ReadString = nullptr;
HL_MSG_ReadCoord MSG_ReadCoord = nullptr;
HL_MSG_ReadBuf MSG_ReadBuf = nullptr;

HL_MSG_ReadBitVec3Coord MSG_ReadBitVec3Coord = nullptr;
HL_MSG_ReadBits MSG_ReadBits = nullptr;
HL_MSG_StartBitReading MSG_StartBitReading = nullptr;
HL_MSG_EndBitReading MSG_EndBitReading = nullptr;
void MSG_SaveReadCount() {
	MSG_SavedReadCount = *MSG_ReadCount;
}

void MSG_RestoreReadCount() {
	*MSG_ReadCount = MSG_SavedReadCount;
}

struct buffer_data
{
	int slot;
	bool ReceiveMicData;
	std::vector<unsigned char> data;
	std::chrono::high_resolution_clock::time_point play_until;
	std::condition_variable cv;
	std::mutex cv_m;
	ao_device* device;
};

cvar_t *voice_scale;
cvar_t* voice_enable;
buffer_data buffer[4];
ISteamUser* g_pISteamUser = nullptr;
VMTHook* hooker = nullptr;
extern int IsHQ;
using byte = std::uint8_t;

struct Regs
{
	union
	{
		std::uint16_t bc;

		struct
		{
			// The order of these bytes matters
			byte c;
			byte b;
		};
	};
};

#define IDEAL_GAIN		4000.0f	// 10 ^ (72 dB / 20)
#define MAX_SCALE		12.0f	// Ignore sample when being scaled over this amount.

void NormalizeClientVoice(int client, char *pVoiceData, int nSamples)
{
	int samplemul = nSamples * 2;
	int maxgain = 0;

	for (int i = 0; i < samplemul; i += 2)
	{
		short sample = ((short)pVoiceData[i+1] << 8) | (pVoiceData[i] & 0xFF);
		sample = abs(sample);

		if (sample > maxgain)
			maxgain = sample;
	}

	if (maxgain == 0)
		return;

	// Save the previous scale to gradually increment over the buffer.
	static float prevscale[4];

	float targetscale = ((voice_scale->value)*IDEAL_GAIN) / maxgain;
	float scale = prevscale[client];

	// Samples can scale incredibly high as the voice lowers between words.
	if (targetscale > MAX_SCALE)
		targetscale = scale;

	float increment = (targetscale - scale) / nSamples;

	// Regain volume over 2 seconds.
	if (targetscale >= 1.0f)
		increment *= (float)nSamples / (48000 * 2);

	for (int i = 0; i < samplemul; i += 2)
	{
		short sample = ((short)pVoiceData[i+1] << 8) | (pVoiceData[i] & 0xFF);

		sample = (short)(sample * scale);
		scale += increment;

		pVoiceData[i+1] = (sample >> 8) & 0xFF;
		pVoiceData[i] = sample & 0xFF;
	}

	prevscale[client] = scale;
}

void voice_thread(int id)
{
	while (1)
	{
		std::chrono::seconds timeoutPeriod = 1s;
		auto timePoint = std::chrono::system_clock::now() + timeoutPeriod;

		std::unique_lock<std::mutex> lk(buffer[id].cv_m);
		buffer[id].cv.wait_for(lk, 500ms, [id] {return buffer[id].ReceiveMicData == true; });
		if (!buffer[id].ReceiveMicData)
		{
			buffer[id].slot = -1;
			continue;
		}
		/*int sampleCount = buffer[id].data.size();
		unsigned short max_vol = 0;
		for (int i = 0; i < sampleCount; i += 2) {
			Regs regs;
			regs.c = buffer[id].data[i];
			regs.b = buffer[id].data[i + 1];
			if(regs.bc >= max_vol)
			{
				max_vol = regs.bc;
			}
		}
		for (int i = 0; i < sampleCount; i+=2) {
			Regs regs;
			regs.c = buffer[id].data[i];
			regs.b = buffer[id].data[i + 1];

			double dsample = (double)regs.bc * max_vol/(1.0 - voice_scale->value);
			if (dsample > 32767.0) { dsample = 32767.0; }
			if (dsample < -32768.0) { dsample = -32768.0; }
			regs.bc = (short)dsample;
			buffer[id].data[i] = regs.c;
			buffer[id].data[i + 1] = regs.b;

			//buffer[id].data[i] -= (int16_t)(buffer[id].data[i] * (1.0-voice_scale->value)); // Should work +/- values
			//buffer[id].data[i+1] -= (int16_t)(buffer[id].data[i+1] * (1.0 - voice_scale->value)); // Should work +/- values
		}*/
		NormalizeClientVoice(id, (char*)buffer[id].data.data(), buffer[id].data.size() / 2);
		ao_play(buffer[id].device, (char*)buffer[id].data.data(), buffer[id].data.size());
		buffer[id].data.clear();
		buffer[id].ReceiveMicData = false;
	}
}
int current_thread = -1;
char dBuffer[900000];
EVoiceResult(__fastcall * DecompressVoice)(ISteamUser * self, int, const void* pCompressed, uint32 cbCompressed, void* pDestBuffer, uint32 cbDestBufferSize, uint32 * nBytesWritten, uint32 nDesiredSampleRate);
EVoiceResult __fastcall DecompressVoice_Hook(ISteamUser * self, int, const void* pCompressed, uint32 cbCompressed, void* pDestBuffer, uint32 cbDestBufferSize, uint32 * nBytesWritten, uint32 nDesiredSampleRate)
{
	EVoiceResult ret;

	size_t writted;
	if (current_thread != -1)
	{

		ret = DecompressVoice(self, 0, pCompressed, cbCompressed, dBuffer, sizeof dBuffer, &writted, 48000);
		buffer[current_thread].data.insert(buffer[current_thread].data.begin(), dBuffer, dBuffer + writted);
		
		*nBytesWritten = cbDestBufferSize-1;

		memset(pDestBuffer, 0, *nBytesWritten);

		buffer[current_thread].ReceiveMicData = true;
		buffer[current_thread].cv.notify_all();
	}

	current_thread = -1;

	return k_EVoiceResultOK;

}

pfnEngineMessage pSVC_VoiceData;

void SVC_VoiceData()
{
	MSG_SaveReadCount();
	int slot = MSG_ReadByte();
	MSG_RestoreReadCount();
	if (voice_enable->value)
	{
		bool found = false;
		for (int i = 0; i < 4; i++)
		{
			if (buffer[i].slot == slot)
			{
				current_thread = i;
				found = true;
				break;
			}
		}
		if (!found)
		{
			for (int i = 0; i < 4; i++)
			{
				if (buffer[i].slot == -1)
				{
					current_thread = i;
					buffer[i].slot = slot;
					break;
				}
			}
		}
	}
	
	pSVC_VoiceData();
}

void VoicePatch()
{
	HMODULE hSteamAPI;

	while (true)
	{
		hSteamAPI = GetModuleHandleA("steam_api.dll");

		if (hSteamAPI)
			break;

		Sleep(500);
	}

	if (hSteamAPI)
	{
		using TSteamUser = ISteamUser * (*)();
		auto pfnSteamUser = (TSteamUser)GetProcAddress(hSteamAPI, "SteamUser");
		g_pISteamUser = pfnSteamUser();
		pEngineMsgBase = (PEngineMsg)offset.FindSVCMessages();
		pSVC_VoiceData = HookEngineMsg("svc_voicedata", SVC_VoiceData);

		voice_scale = g_Engine.pfnGetCvarPointer("voice_scale");
		voice_enable = g_Engine.pfnGetCvarPointer("voice_enable");
		hooker = new VMTHook((void*)g_pISteamUser);
		DecompressVoice = hooker->GetOriginalFunction<decltype(DecompressVoice)>(11);
		hooker->HookFunction(DecompressVoice_Hook, 11);

		ao_initialize();
		int driver = ao_default_driver_id();
		// The format of the decoded PCM samples
		ao_sample_format sample_format;
		sample_format.bits = 16;
		sample_format.channels = 1;
		sample_format.rate = 48000;
		sample_format.byte_format = AO_FMT_NATIVE;
		sample_format.matrix = 0;

		for(int i = 0; i < 4; i++)
		{

			

			buffer[i].device = ao_open_live(driver, &sample_format, NULL);
			buffer[i].slot = -1;
			buffer[i].ReceiveMicData = false;
			std::thread(voice_thread, i).detach();
			
		}	
	}
}