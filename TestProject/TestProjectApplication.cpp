// Copyright 2016 Scott Bevin, All Rights Reserved

#include "TestProjectApplication.h"

#include "AX/Core/AXLogging.h"

/**
* Constructor
*/
TestProjectApplication::TestProjectApplication()
{

}

/**
* Destructor
*/
TestProjectApplication::~TestProjectApplication()
{

}

/**
* Creates game systems
*/
void TestProjectApplication::CreateDefaultSystems( )
{
	if( AXLogging* logger = CreateSystem< AXLogging >( ) )
	{
		logger->RegisterNewListener< AXLogListener_ConsoleOutput >( );
	}

	AXApplication::CreateDefaultSystems( );
}