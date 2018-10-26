// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXApplication.h"
#include "AXWindow.h"
#include "AXLogging.h"
#include "AXSettings.h"
#include "AX/Graphics/RenderCore/AXRenderCore.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AX/Content/AXContent.h"
#include "AX/Editor/AXEditor.h"
#include "AX/Core/Threads/AXThreading.h"
#include "AX/Core/Threads/AXThreadedTasks.h"

#include <iterator>
#include <vector>
#include <chrono>
#include <thread>

/**
* Constructor
*/
AXApplication::AXApplication( )
{

}

/**
* Destructor
*/
AXApplication::~AXApplication( )
{

}

/**
* Initialises the engine, performs update loop then destroys the engine at the appropriate time
* and returns out.
*/
void AXApplication::RunEngine( const SetupParams& prams )
{
	if( Startup( prams ) )
	{
		RunEngineLoop( );
	}

	Shutdown( );
}

/**
* Initializes the engine
*/
bool AXApplication::Startup( const SetupParams& prams )
{
	AXLOG( "Application", "Starting up engine" );

	CreateDefaultSystems( );

	LoadSettings( );

	HandleCommandLine( prams.mCommandLine );

	if( !InitializeAllSystems( ) )
	{
		return false;
	}

	if( AXSettings* settings = AXSettings::GetFrom( *this ) )
	{
		settings->GetEngineSettingsFile().Save( );
	}	

	AXLOG( "Application", "Finished engine startup" );

	return true;
}

/**
* Runs the engine loop
*/
void AXApplication::RunEngineLoop( )
{
	AXLOG( "Application", "Starting engine loop" );

	Settings& appSettings( GetSettings( ) );
	
	std::chrono::duration<double, std::milli> frameDeltaTime( 0 );

	do 
	{
		auto frameStartTime( std::chrono::high_resolution_clock::now( ) );
		float dt( ( float )frameDeltaTime.count( ) * 0.001f );

		for( auto it : GetSystems( ) )
		{
			it->BeginFrame( );
		}

		for( auto it : GetSystems( ) )
		{
			it->Update( dt );
		}

		for( auto it : GetSystems( ) )
		{
			it->Render( );
		}

		for( auto it : GetSystems( ) )
		{
			it->EndFrame( );
		}

		//////////////////////////////////////////////////////////////////////////
		// Frame capping

		auto frameEndTime( std::chrono::high_resolution_clock::now( ) );
		
		frameDeltaTime = frameEndTime - frameStartTime;

 		if( appSettings.mMaxFPS > 0 )
 		{
 			std::chrono::duration<double, std::milli>  minFrameTime( 1000.0f / ( double )appSettings.mMaxFPS );
 
			if( minFrameTime > frameDeltaTime )
 			{
				minFrameTime -= frameDeltaTime;

				std::this_thread::sleep_for( std::chrono::milliseconds( (long long)minFrameTime.count() ) );
 				frameDeltaTime = minFrameTime;
 			}
 		}

	} while ( !mQuitEngine );

	AXLOG( "Application", "Finished engine loop" );
}

/**
* Shuts down the engine
*/
void AXApplication::Shutdown( )
{
	AXLOG( "Application", "Shutting down engine" );

	ShutdownAllSystems( );
}


/**
* Creates settings objects for all systems then loads settings
*/
void AXApplication::LoadSettings( )
{
	AXLOG( "Application", "Loading Settings" );

	if( AXSettings* settings = AXSettings::GetFrom( *this ) )
	{
		mAppSettings = settings->GetEngineSettingsFile( ).RegisterNewItem< Settings >( "Application" );

		for( auto it : GetSystems( ) )
		{
			it->CreateEngineSettings( settings->GetEngineSettingsFile() );

			AXLOG( "Application", " - Created settings object: %s", it->GetName().c_str() );
		}

		settings->GetEngineSettingsFile( ).Load( );
	}
	else
	{
		AXERROR( "Application", "Failed to find settings system." );
	}
}

/**
* Passes command line arguments to all systems
*/
void AXApplication::HandleCommandLine( const AXString& commandLine )
{
	AXLOG( "Application", "Handling command line: %s", commandLine.c_str() );

	std::vector<AXString> args;
	AXUtils::SplitString( commandLine, ' ', args );

	for( auto it : GetSystems( ) )
	{
		it->HandleCommandLine( args );
	}
}

/**
* Goes through all systems and intiializes them
*/
bool AXApplication::InitializeAllSystems( )
{
	bool needToRetry = false;

	do 
	{
		needToRetry = false;
		
		for( auto it : GetSystems( ) )
		{
			if( it->GetState() == AXSystemBase::State::Uninitialized )
			{
				AXSystemBase::InitResult res( it->Initialize( ) );

				if( res == AXSystemBase::InitResult::Initialized )
				{
					AXLOG( "Application", "Initialized system: %s", it->GetName().c_str() );
				}
				else if( res == AXSystemBase::InitResult::Failed )
				{
					AXERROR( "Application", "Failed to initialize system: %s", it->GetName( ).c_str( ) );
					return false;
				}
				else if( res == AXSystemBase::InitResult::Retry )
				{
					needToRetry = true;
				}
			}
		}

	} while ( needToRetry );


	return true;
}

/**
* Goes through all systems and intiializes them
*/
void AXApplication::ShutdownAllSystems( )
{
	auto& systems = GetSystems( );

	for( auto it = systems.rbegin( ); it != systems.rend( ); ++it )
	{
		AXLOG( "Application", "Shutdown system: %s", (*it)->GetName( ).c_str( ) );
		( *it )->Shutdown( );
	}

	CleanupSystems( );
}

/**
* Creates game systems
*/
void AXApplication::CreateDefaultSystems( )
{
	AXLOG( "Application", "Creating default systems" );

	mUpdatablesSystem = CreateSystem< AXUpdateables >( );

	CreateSystem< AXSettings >( );
	CreateSystem< AXWindow >( );
	CreateSystem< AXRenderCore >( );
	CreateSystem< AXImGui >( );
	CreateSystem< AXContent >( );
	CreateSystem< AXEditor >( );
	CreateSystem< AXThreading >( );
	CreateSystem< AXThreadedTasks >( );
}