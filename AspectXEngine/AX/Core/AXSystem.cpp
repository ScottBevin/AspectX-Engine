// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXSystem.h"

/**
* Constructor
*/
AXSystemBase::AXSystemBase( const AXString& name )
	: mName( name )
{

}

/**
* Destructor
*/
AXSystemBase::~AXSystemBase()
{

}

/**
* Initialise the system, called after settings are loaded
*/
AXSystemBase::InitResult AXSystemBase::Initialize( )
{
	InitResult res( OnInitialize( ) );

	if( res == InitResult::Initialized )
	{
		mState = State::Initialized;
	}
	else if( res == InitResult::Failed )
	{
		mState = State::FailedToInitialize;
	}

	return res;
}

/**
* Shutdown the system
*/
void AXSystemBase::Shutdown( )
{
	if( mState == State::Initialized || mState == State::FailedToInitialize )
	{
		OnShutdown( );
	}

	mState = State::Shutdown;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Attempts to find a system, if registered
*/
const AXSystemBase* AXISystemOwner::FindSystem( const AXString& sysName ) const
{
	for( auto it : mSystems )
	{
		if( it->GetName( ) == sysName )
		{
			return it;
		}
	}

	return nullptr;
}

/**
* Attempts to find a system, if registered
*/
AXSystemBase* AXISystemOwner::FindSystem( const AXString& sysName )
{
	for( auto it : mSystems )
	{
		if( it->GetName( ) == sysName )
		{
			return it;
		}
	}

	return nullptr;
}
/**
* Deletes all systems
*/
void AXISystemOwner::CleanupSystems( )
{
	for( auto it = mSystems.rbegin( ); it != mSystems.rend( ); ++it )
	{
		delete *it;
	}

	mSystems.clear( );
}
