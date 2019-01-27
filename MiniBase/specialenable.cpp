#include "main.h"

void CMDHandler_Special();

void SpecialEnable()
{
	// add '_special' support for 48 protocol
//	if(protocol_48)
		g_pEngine->pfnAddCommand("special", CMDHandler_Special);
}

void CMDHandler_Special()
{
	g_pEngine->pfnClientCmd("_special");
}