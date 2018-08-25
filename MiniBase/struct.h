#pragma once

#include "main.h"

typedef void(*pfnEngineMessage)();
typedef void(*pfnEventMsgHook)(event_args_t *args);

typedef unsigned __int16 uint16;

typedef struct Color24
{
	BYTE R, G, B;
} TColor24, *PColor24;

typedef struct UserMsg
{
	int number;
	int size;
	char name[16];
	struct UserMsg* next;
	pfnUserMsgHook pfn;
} *PUserMsg;

typedef struct EngineMsg
{
	int number;
	char* name;
	pfnEngineMessage pfn;
} *PEngineMsg;
typedef struct EventMsg
{
	struct EventMsg* next;
	char* name;
	pfnEventMsgHook pfn;
} *PEventMsg;
typedef struct cmd_s
{
	struct cmd_s *next;
	char *name;
	xcommand_t function;
	int flags;
} cmd_t, *pcmd_t;

typedef struct sizebuf_s
{
	const char *buffername;
	uint16 flags;
	byte *data;
	int maxsize;
	int cursize;
} sizebuf_t;

typedef struct GameInfo_s
{
	char*		GameName;
	char*		GameVersion;
	BYTE		Protocol;
	DWORD		Build;
} GameInfo_t, *pGameInfo_s;

typedef struct cl_clientfuncs_s
{
	int(*Initialize) (cl_enginefunc_t *pEnginefuncs, int iVersion);
	int(*HUD_Init) (void);
	int(*HUD_VidInit) (void);
	void(*HUD_Redraw) (float time, int intermission);
	int(*HUD_UpdateClientData) (client_data_t *pcldata, float flTime);
	int(*HUD_Reset) (void);
	void(*HUD_PlayerMove) (struct playermove_s *ppmove, int server);
	void(*HUD_PlayerMoveInit) (struct playermove_s *ppmove);
	char(*HUD_PlayerMoveTexture) (char *name);
	void(*IN_ActivateMouse) (void);
	void(*IN_DeactivateMouse) (void);
	void(*IN_MouseEvent) (int mstate);
	void(*IN_ClearStates) (void);
	void(*IN_Accumulate) (void);
	void(*CL_CreateMove) (float frametime, struct usercmd_s *cmd, int active);
	int(*CL_IsThirdPerson) (void);
	void(*CL_CameraOffset) (float *ofs);
	struct kbutton_s *(*KB_Find) (const char *name);
	void(*CAM_Think) (void);
	void(*V_CalcRefdef) (struct ref_params_s *pparams);
	int(*HUD_AddEntity) (int type, struct cl_entity_s *ent, const char *modelname);
	void(*HUD_CreateEntities) (void);
	void(*HUD_DrawNormalTriangles) (void);
	void(*HUD_DrawTransparentTriangles) (void);
	void(*HUD_StudioEvent) (const struct mstudioevent_s *event, const struct cl_entity_s *entity);
	void(*HUD_PostRunCmd) (struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
	void(*HUD_Shutdown) (void);
	void(*HUD_TxferLocalOverrides) (struct entity_state_s *state, const struct clientdata_s *client);
	void(*HUD_ProcessPlayerState) (struct entity_state_s *dst, const struct entity_state_s *src);
	void(*HUD_TxferPredictionData) (struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
	void(*Demo_ReadBuffer) (int size, unsigned char *buffer);
	int(*HUD_ConnectionlessPacket) (struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
	int(*HUD_GetHullBounds) (int hullnumber, float *mins, float *maxs);
	void(*HUD_Frame) (double time);
	int(*HUD_Key_Event) (int down, int keynum, const char *pszCurrentBinding);
	void(*HUD_TempEntUpdate) (double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int(*Callback_AddVisibleEntity)(struct cl_entity_s *pEntity), void(*Callback_TempEntPlaySound)(struct tempent_s *pTemp, float damp));
	struct cl_entity_s *(*HUD_GetUserEntity) (int index);
	int(*HUD_VoiceStatus) (int entindex, qboolean bTalking);
	int(*HUD_DirectorMessage) (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
	int(*HUD_GetStudioModelInterface) (int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio);
	void(*HUD_CHATINPUTPOSITION_FUNCTION) (int *x, int *y);
	int(*HUD_GETPLAYERTEAM_FUNCTION) (int iplayer);
	void(*CLIENTFACTORY) (void);
} cl_clientfunc_t;

#define	SVC_BAD                   0
#define	SVC_NOP                   1
#define	SVC_DISCONNECT            2
#define	SVC_EVENT                 3
#define	SVC_VERSION               4
#define	SVC_SETVIEW               5
#define	SVC_SOUND                 6
#define	SVC_TIME                  7
#define	SVC_PRINT                 8
#define	SVC_STUFFTEXT             9
#define	SVC_SETANGLE              10
#define	SVC_SERVERINFO            11
#define	SVC_LIGHTSTYLE            12
#define	SVC_UPDATEUSERINFO        13
#define	SVC_DELTADESCRIPTION      14
#define	SVC_CLIENTDATA            15
#define	SVC_STOPSOUND             16
#define	SVC_PINGS                 17
#define	SVC_PARTICLE              18
#define	SVC_DAMAGE                19
#define	SVC_SPAWNSTATIC           20
#define	SVC_EVENT_RELIABLE        21
#define	SVC_SPAWNBASELINE         22
#define	SVC_TEMPENTITY            23
#define	SVC_SETPAUSE              24
#define	SVC_SIGNONNUM             25
#define	SVC_CENTERPRINT           26
#define	SVC_KILLEDMONSTER         27
#define	SVC_FOUNDSECRET           28
#define	SVC_SPAWNSTATICSOUND      29
#define	SVC_INTERMISSION          30
#define	SVC_FINALE                31
#define	SVC_CDTRACK               32
#define	SVC_RESTORE               33
#define	SVC_CUTSCENE              34
#define	SVC_WEAPONANIM            35
#define	SVC_DECALNAME             36
#define	SVC_ROOMTYPE              37
#define	SVC_ADDANGLE              38
#define	SVC_NEWUSERMSG            39
#define	SVC_PACKETENTITIES        40
#define	SVC_DELTAPACKETENTITIES   41
#define	SVC_CHOKE                 42
#define	SVC_RESOURCELIST          43
#define	SVC_NEWMOVEVARS           44
#define	SVC_RESOURCEREQUEST       45
#define	SVC_CUSTOMIZATION         46
#define	SVC_CROSSHAIRANGLE        47
#define	SVC_SOUNDFADE             48
#define	SVC_FILETXFERFAILED       49
#define	SVC_HLTV                  50
#define	SVC_DIRECTOR              51
#define	SVC_VOICEINIT             52
#define	SVC_VOICEDATA             53
#define	SVC_SENDEXTRAINFO         54
#define	SVC_TIMESCALE             55
#define	SVC_RESOURCELOCATION      56
#define	SVC_SENDCVARVALUE         57
#define	SVC_SENDCVARVALUE2        58