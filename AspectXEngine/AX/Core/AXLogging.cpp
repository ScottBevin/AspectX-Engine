// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXLogging.h"
#include "AXApplication.h"

#include <windows.h>
#include <strstream>

AXString AXSystem< AXLogging >::sSystemName = "Logging";

/**
* Constructor
*/
AXLogging::AXLogging()
{
	
}
	
/**
* Destructor
*/
AXLogging::~AXLogging()
{
	
}

/**
* Shutdown the system
*/
void AXLogging::OnShutdown( )
{
 	for( auto listener : mListeners )
 	{
 		delete listener;
 	}
	
	mListeners.clear( );
}

/**
* Helper function to return the logging system
*/
AXLogging* AXLogging::Get( )
{
	return AXLogging::GetFrom( AXApplication::Get( ) );
}

/**
* Override to register a settings object for this system
*/
void AXLogging::CreateEngineSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXLogging::Settings >( AXLogging::StaticName( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Built-in loggers

/**
* Override to handle a log entry
*/
void AXLogListener_ConsoleOutput::Log( const AXLogging::LogEntry& entry )
{
	printf( AXUtils::FormatString( "[%-8s] [%-16s] %s (%s : %d) \n",
		AXLogging::LogLevel::ToString( entry.mLogLevel ).c_str( ),
		entry.mTag.c_str( ),
		entry.mMessage.c_str( ),
		entry.mFile.c_str( ),
		entry.mLine ).c_str( ) );
	fflush( stdout );
}