// Copyright 2016 Scott Bevin, All Rights Reserved

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "TestProjectApplication.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow )
{
	AXApplication::SetupParams prams;
	prams.mCommandLine = pScmdline;

	AXApplication::Use< TestProjectApplication >( ).RunEngine( prams );

	return 0;
}