// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXThreading.h"
#include "AX/Core/AXLogging.h"
#include "AX/Core/AXApplication.h"

AXString AXSystem< AXThreading >::sSystemName = "Threading";

static AXString sAXDefaultThreadName = "AX Thread";

/**
* Gets set to notify spawned threads that they should stop running
*/
bool AXThreading::sShuttingDown = false;

/**
* Initialise the system, called after settings are loaded
*/
AXThreading::InitResult AXThreading::OnInitialize( )
{
	uint32_t maxConcurentThreads( std::thread::hardware_concurrency( ) );

	if( maxConcurentThreads < 2 )
	{
		return AXThreading::InitResult::Failed;
	}

	// Create max - 1 so that we dont create one on the same core as the main thread...
	mThreadPool = new ThreadPool( maxConcurentThreads - 1 );

	if( !mThreadPool )
	{
		return AXThreading::InitResult::Failed;
	}

	for( AXThread& item : *mThreadPool )
	{
		if( item.mNativeThread = new std::thread( NativeThreadFunc, std::ref( item ) ) )
		{
			item.SetThreadName( sAXDefaultThreadName );
		}
	}

	if( AXImGui* imGui = AXImGui::GetFrom( AXApplication::Get( ) ) )
	{
		imGui->RegisterSystemDebugMenuItem( "Window/Threading/Thread Stats", std::bind( &AXThreading::ImGuiThreadingWindowCallback, this, std::placeholders::_1 ) );
	}

	return AXThreading::InitResult::Initialized;
}

/**
* Called once a frame to allow systems to update
*/
void AXThreading::Update( float dt )
{
	RenderImGuiThreadingWindow( );
}

/**
* Shutdown the system
*/
void AXThreading::OnShutdown( )
{
	sShuttingDown = true;

	AXLOG( "Threads", "Waiting for threads to shut down." );

	if( mThreadPool )
	{
		for( AXThread& item : *mThreadPool )
		{
			if( item.mNativeThread )
			{
				item.mNativeThread->join( );
				delete item.mNativeThread;
				item.mNativeThread = nullptr;
			}
		}
	}

	AXLOG( "Threads", "All threads shut down" );

	delete mThreadPool;
	mThreadPool = nullptr;
}

/**
* The native callback that keeps the threads running
*/
void AXThreading::NativeThreadFunc( AXThread& thread )
{
	while( !thread.mNativeThread ) { }

	AXLOG( "Threads", "Starting thread: %d", thread.mNativeThread->get_id( ) );

	do 
	{
		bool didSomething( false );

		if( thread.mParams.mCallback )
		{
			thread.mState = AXThread::State::Running;
			didSomething = true;

			ThreadResult result( thread.mParams.mCallback( thread.mParams.mUserData ) );

			if( result.mResult == ThreadResult::Result::Finish )
			{
				if( AXThreading* threading = AXThreading::GetFrom( AXApplication::Get( ) ) )
				{
					threading->ReleaseThread( thread.mHandle );
				}
			}
		}

	} while ( !sShuttingDown );

	AXASSERT( thread.mNativeThread, "Native thread invalid" );
	AXLOG( "Threads", "Shutting down thread: %d", thread.mNativeThread->get_id( ) );
}

/**
* Obtains a thread taking permanent control over it
*/
AXThreading::ThreadHandle AXThreading::ObtainThread( const ObtainThreadParams& params )
{
	if( params.mCallback )
	{
		AXThread* obtainedThread = nullptr;
		ThreadHandle hndl( mThreadPool->Allocate( &obtainedThread ) );

		if( hndl.IsValid( ) && obtainedThread )
		{
			obtainedThread->mHandle = hndl;
			obtainedThread->mParams = params;

			obtainedThread->SetThreadName( params.mThreadName );

			return hndl;
		}
	}

	return ThreadHandle::Invalid;
}

/**
* Releases a thread, releasing permanent control over it
*/
void AXThreading::ReleaseThread( ThreadHandle& handle )
{
	if( AXThread* thread = mThreadPool->TryGet( handle ) )
	{
		thread->mParams = ObtainThreadParams( );
		thread->mState = AXThread::State::Available;

		thread->SetThreadName( sAXDefaultThreadName );
	}

	mThreadPool->Release( handle );
}

/**
* A callback function to draw the threading window
*/
void AXThreading::ImGuiThreadingWindowCallback( AXImGui::SystemDebugMenuItem& item )
{
	ImGui::MenuItem( item.mText.c_str( ), "", &mShouldRenderImGuiOverviewWindow );
}

/**
* Renders the IM gui threading editing window
*/
void AXThreading::RenderImGuiThreadingWindow( )
{
	if( mShouldRenderImGuiOverviewWindow )
	{
		if( ImGui::Begin( "Thread Stats", &mShouldRenderImGuiOverviewWindow ) )
		{
			ImGui::BeginChild( "left pane", ImVec2( 250, 0 ), true );

			static AXThread* CurrentlySelectedThread = nullptr;

			uint32_t id( 0 );
			for( AXThread& item : *mThreadPool )
			{
				ImGui::PushID( id++ );

				if( ImGui::Selectable( AXUtils::FormatString( "Thread %d: %s", id - 1, item.mParams.mThreadName.c_str() ).c_str(), CurrentlySelectedThread == &item ) )
				{
					CurrentlySelectedThread = &item;
				}

				ImGui::PopID( );
			}

			ImGui::EndChild( );

			ImGui::SameLine( );

			ImGui::BeginGroup( );

			ImGui::BeginChild( "item view", ImVec2( 0, -ImGui::GetItemsLineHeightWithSpacing( ) ) );

			if( CurrentlySelectedThread )
			{
				ImGui::Text( CurrentlySelectedThread->mParams.mThreadName.c_str( ) );
				ImGui::Separator( );

				ImGui::Text( "State: %s", AXThread::State::ToString( CurrentlySelectedThread->mState ).c_str() );
			}

			ImGui::EndChild( );

			ImGui::EndGroup( );
		}

		ImGui::End( );
	}
}