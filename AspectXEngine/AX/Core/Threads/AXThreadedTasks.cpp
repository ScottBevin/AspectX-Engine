// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXThreadedTasks.h"
#include "AX/Core/AXApplication.h"

AXString AXSystem< AXThreadedTasks >::sSystemName = "Threaded Tasks";

AXTask::TaskResult TestTask( AXTask::TaskUserData* userData )
{
	int test = 1;
	++test;

	return AXTask::TaskResult( );
}

/**
* Constructor
*/
AXThreadedTasks::AXThreadedTasks()
{

}

/**
* Destructor
*/
AXThreadedTasks::~AXThreadedTasks()
{

}

/**
* Initialise the system, called after settings are loaded
*/
AXThreadedTasks::InitResult AXThreadedTasks::OnInitialize( )
{
	if( AXThreading* threading = AXThreading::GetFrom( AXApplication::Get( ) ) )
	{
		if( threading->GetState( ) != AXSystemBase::State::Initialized )
		{
			return AXSystemBase::InitResult::Retry;
		}

		if( mSettings->mNumDedicatedThreads > threading->MaxThreads( ) )
		{
			mSettings->mNumDedicatedThreads = threading->MaxThreads( );
		}

		AXThreading::ObtainThreadParams params;
		params.mCallback = std::bind( &AXThreadedTasks::ThreadCallbackFunc, this, std::placeholders::_1 );
		params.mUserData = nullptr;

		for( uint8_t i( 0 ); i < mSettings->mNumDedicatedThreads; ++i )
		{
			params.mThreadName = AXUtils::FormatString( "Threaded Tasks dedicated thread %d.", i );

			mDedicatedThreads.push_back( threading->ObtainThread( params ) );
		}
	}

	return AXThreadedTasks::InitResult::Initialized;
}

/**
* Called once a frame to allow systems to update
*/
void AXThreadedTasks::Update( float dt )
{

}

/**
* Shutdown the system
*/
void AXThreadedTasks::OnShutdown( )
{
	if( AXThreading* threading = AXThreading::GetFrom( AXApplication::Get( ) ) )
	{
		for( auto& threadHndl : mDedicatedThreads )
		{
			threading->ReleaseThread( threadHndl );
		}
	}

	mDedicatedThreads.clear( );
}

/**
* Adds a task into the task queue to run at some point on some thread
*/
void AXThreadedTasks::RequestTaskRun( const AXTask::Params& params )
{
	if( params.mCallback )
	{
		AXTask* newTask = new AXTask( params );

		mTasks.GetWrite( &params ).push_back( newTask );
		mTasks.ReleaseLock( &params );
	}
}

/**
* Performs the next available task on the calling thread, returns true if a task was run
*/
bool AXThreadedTasks::RunNextAvailableTask( )
{
	// An object that can be used for locking tasks to
	static thread_local int TasksLockObj = 0;

	// Will read lock the object
	if( mTasks.GetRead( &TasksLockObj ).empty( ) )
	{
		mTasks.ReleaseLock( &TasksLockObj );
		return false;
	}

	AXTask* taskToRun( nullptr );

	{
		// Lock the object for writing
		if( TaskCollection* tasks = mTasks.TryGetWrite( &TasksLockObj ) )
		{
			// Go through all tasks reducing priority by 1, pick the lowest
			for( AXTask* task : *tasks )
			{
				task->mPriorityCounter--;

				if( !taskToRun || task->mPriorityCounter < taskToRun->mPriorityCounter )
				{
					taskToRun = task;
				}
			}

			if( taskToRun )
			{
				tasks->remove( taskToRun );
			}

			mTasks.ReleaseLock( &TasksLockObj );
		}
	}

	if( taskToRun )
	{
		taskToRun->mParams.mCallback( taskToRun->mParams.mUserData );

		delete taskToRun->mParams.mUserData;
		delete taskToRun;

		return true;
	}

	return false;
}

/**
* Override to register a settings object for this system
*/
void AXThreadedTasks::CreateEngineSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXThreadedTasks::Settings >( AXThreadedTasks::StaticName( ) );
}

/**
* The callback function we supply to the treading system
*/
AXThreading::ThreadResult AXThreadedTasks::ThreadCallbackFunc( AXThreading::ThreadUserData* userData )
{
	RunNextAvailableTask( );

	AXThreading::ThreadResult result;
	result.mResult = AXThreading::ThreadResult::Result::ReRun;

	return result;
}