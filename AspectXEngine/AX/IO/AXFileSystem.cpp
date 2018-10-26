// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXFileSystem.h"

AXString AXSystem< AXFileSystem >::sSystemName = "Files";

/**
* Initialise the system, called after settings are loaded
*/
AXFileSystem::InitResult AXFileSystem::OnInitialize( )
{
	return AXFileSystem::InitResult::Initialized;
}

/**
* Called once a frame to allow systems to update
*/
void AXFileSystem::Update( float dt )
{

}

/**
* Shutdown the system
*/
void AXFileSystem::OnShutdown( )
{

}

/**
* Creates a new file and queues it for an async read, returns a pointer to the file that
* must be destroyed by calling DestroyFile() when finished.
*/
class AXFile* AXFileSystem::ReadFile( const FileTransactionParams& params )
{
	return nullptr;
}

/**
* Cleanly destroys the file and any associated memory in a thread-safe manner
*/
void AXFileSystem::DestroyFile( class AXFile& file )
{

}

/**
* Override to register a settings object for this system
*/
void AXFileSystem::CreateEngineSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXFileSystem::Settings >( AXFileSystem::StaticName( ) );
}
