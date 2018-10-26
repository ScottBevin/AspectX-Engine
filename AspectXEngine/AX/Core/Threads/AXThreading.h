// Copyright 2016 Scott Bevin, All Rights Reserved
#pragma once

#include "AX/Core/AXSystem.h"
#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXThreadingPrimitives.h"
#include "AX/Utils/AXHandle.h"
#include "AX/Utils/AXResourcePool.h"

#include <functional>

class AXThreading : public AXParent< AXSystem< AXThreading >, AXThreading >
{
public:

	struct ThreadResult
	{
		enum class Result
		{
			ReRun,
			RerunDelay,
			Finish,
		};

		Result mResult = Result::Finish;
		float mDelayTime = 0.0f;
	};

	class ThreadUserData
	{

	};

	using ThreadCallback = std::function< ThreadResult( ThreadUserData* ) >;

	struct ObtainThreadParams
	{
		ObtainThreadParams( )
		{
			mCallback = nullptr;
			mUserData = nullptr;
			mThreadName = "Available";
		}

		ThreadCallback mCallback;
		ThreadUserData* mUserData;
		AXString mThreadName;
	};

	using ThreadHandle = AXHandleBase< uint8_t, uint8_t >;

private:
	class AXThread
	{
	public:

		struct State
		{
			enum E
			{
				Available,
				Running,
				Waiting,
				RunningTask,
			};

			static const AXString& ToString( E e )
			{
				static AXString strings[] = { "Available", "Running", "Waiting", "Running task" };
				return strings[e];
			}
		};

	public:
		// The native underlying thread
		std::thread* mNativeThread = nullptr;

		// Params passed in when the thread was obtained
		ObtainThreadParams mParams;

		// A valid handle for this thread object (when the thread is obtained)
		ThreadHandle mHandle = ThreadHandle::Invalid;

		// The current state of this thread
		State::E mState = State::Available;

		// The name given to this thread
		AXString mName;

	public:
		// Sets the name of a thread
		void SetThreadName( const AXString& name );
	};

	using ThreadPool = AXFixedSizeResourcePool< AXThread, ThreadHandle >;

public:
	/**
	* Initialise the system, called after settings are loaded
	*/
	virtual InitResult OnInitialize( ) override;

	/**
	* Called once a frame to allow systems to update
	*/
	virtual void Update( float dt ) override;

	/**
	* Shutdown the system
	*/
	virtual void OnShutdown( ) override;

	/**
	 * Obtains a thread taking permanent control over it
	 */
	ThreadHandle ObtainThread( const ObtainThreadParams& params );

	/**
	 * Releases a thread, releasing permanent control over it and invalidating handle
	 */
	void ReleaseThread( ThreadHandle& handle );

	/**
	* Puts the thread to sleep for the given amount of time, will happen immediately, therefore if
	* called on a thread while in that threads execution the thread will stall. milliseconds < 0.0
	* will sleep indefinitely
	*/
	void Sleep( const ThreadHandle& handle, float milliseconds );

	/**
	* Wakes up a sleeping thread
	*/
	void Wake( const ThreadHandle& handle );

	/**
	 * Returns the number of threads available
	 */
	ThreadHandle::IdType MaxThreads( ) const { return mThreadPool->Capacity( ); }

private:
	/**
	 * The native callback that keeps the threads running
	 */
	static void NativeThreadFunc( AXThread& thread );

	/**
	* A callback function to draw the threading window
	*/
	void ImGuiThreadingWindowCallback( AXImGui::SystemDebugMenuItem& item );

	/**
	* Renders the IM gui threading editing window
	*/
	void RenderImGuiThreadingWindow( );

private:
	/**
	 * The pool containing all available threads
	 */
	ThreadPool* mThreadPool = nullptr;

	/**
	 * Gets set to notify spawned threads that they should stop running
	 */
	static bool sShuttingDown;


	/**
	* If true the threading ImGui window will render
	*/
	bool mShouldRenderImGuiOverviewWindow = false;
};