#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include "main.h"
#include "gl/GL.h"
void (APIENTRY* qglFrustum)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) = NULL;
float(__cdecl* V_CalcFov)(float fov_x, float width, float height);
void(__cdecl* MYgluPerspective_Orig)(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
float new_x;
float new_y;
float frustum_skew = 0.0;
SCREENINFO g_Screen;
void __cdecl MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	float xmax, ymax;
	xmax = zNear * tan(new_x * M_PI / 360.0);
	ymax = zNear * tan(new_y * M_PI / 360.0);
	qglFrustum(-xmax, xmax, -ymax, ymax, zNear, zFar);
}

float __cdecl CalcFov(float fov_x, float width, float height)
{
	return  new_y;
}

int(HUD_UpdateClientData)(client_data_t* pcldata, float flTime)
{
	auto ret = g_Client.HUD_UpdateClientData(pcldata, flTime);
	g_Engine.pfnGetScreenInfo(&g_Screen);
	if (g_Screen.iWidth * 3 == 4 * g_Screen.iHeight || g_Screen.iWidth * 4 == g_Screen.iHeight * 5)
	{
		// 4:3 or 5:4 ratio
		new_x = pcldata->fov;
		new_y = V_CalcFov(pcldata->fov, g_Screen.iWidth, g_Screen.iHeight);
	}
	else
	{
		new_y = V_CalcFov(pcldata->fov, g_Screen.iWidth / 1024.0, g_Screen.iHeight / 768.0);
		new_x = pcldata->fov = V_CalcFov(new_y, g_Screen.iHeight, g_Screen.iWidth);
	}
	return ret;
}

void widescreen_patch()
{
	g_Screen.iSize = sizeof(SCREENINFO);
	auto found = PatternScan::FindPattern("[E8 *? ? ? ?] A1 ? ? ? ? 83 C4 0C D9 5D F4", "hw.dll");
	V_CalcFov = decltype(V_CalcFov)(DetourFunction((PBYTE)found, (PBYTE)CalcFov));
	found = PatternScan::FindPattern("55 8B EC 83 EC 10 DD 45 08", "hw.dll");
	MYgluPerspective_Orig = decltype(MYgluPerspective_Orig)(DetourFunction((PBYTE)found, (PBYTE)MYgluPerspective));
	HMODULE hOpenGL = GetModuleHandle("opengl32.dll");
	*(FARPROC*)&qglFrustum = GetProcAddress(hOpenGL, "glFrustum");
	g_pClient->HUD_UpdateClientData = HUD_UpdateClientData;
}