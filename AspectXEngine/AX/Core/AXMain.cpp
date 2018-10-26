// Copyright 2016 Scott Bevin, All Rights Reserved

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "AX/Core/AXApplication.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow )
{
	AXApplication::SetupParams prams;
	prams.mCommandLine = pScmdline;

	AXApplication::Get( ).RunEngine( prams );

	return 0;
}