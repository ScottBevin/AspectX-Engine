// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AXThreading.h"
#include "AX/Utils/AXParent.h"
#include "AX/Core/AXSystem.h"
#include "AX/Utils/AXThreadingPrimitives.h"

class AXTask : public AXParent< AXBaseObject, AXTask >
{
public:

	struct Priority
	{
		enum E
		{
			ASAP,
			VeryHigh,
			High,
			Normal,
			Low,
			VeryLow,
		};

		static const std::string& ToString( E e )
		{
			static std::string strings[] = { "ASAP", "VeryHigh", "High", "Normal", "Low", "VeryLow" };
			return strings[e];
		}
	};

	struct TaskResult
	{
		enum class Result
		{
			Done,
		};

		Result mResult = Result::Done;
	};

	class TaskUserData
	{

	};

	using TaskCallback = std::function< TaskResult( TaskUserData* ) >;

	struct Params
	{
		/**
		 * The function that should be called by this task
		 */
		TaskCallback mCallback = nullptr;

		// The priority this task should run at
		Priority::E mPriority = Priority::Normal;

		/**
		 * Can point to some allocated user data, the task system assumes responsibility for allocated memory
		 */
		TaskUserData* mUserData = nullptr;

	};

	friend class AXThreadedTasks;

public:

	AXTask( const Params& prams )
		: mParams( prams )
	{
		mPriorityCounter = ( prams.mPriority * 1000 );
	}

private:

	/**
	 * The params we were created with
	 */
	Params mParams;
	 
	/**
	 * A counter that gets counted down when deciding which task to run
	 */
	int32_t mPriorityCounter = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXThreadedTasks : public AXParent< AXSystem< AXThreadedTasks >, AXThreadedTasks >
{
public:
	class Settings : public AXParent< AXSettingsFile::SettingsItem, Settings >
	{
	public:

		/**
		* Constructor
		*/
		Settings( )
		{
			RegisterProperty( mNumDedicatedThreads, "Dedicated Threads" );
		}

	public:
		AXProperty< uint8_t > mNumDedicatedThreads = 1;
	};

public:
	/**
	* Constructor
	*/
	AXThreadedTasks();

	/**
	* Destructor
	*/
	~AXThreadedTasks();
	
	Settings& GetSettings( ) { return AXUtils::AssertPtrReturnRef( mSettings ); }

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
	 * Adds a task into the task queue to run at some point on some thread
	 */
	void RequestTaskRun( const AXTask::Params& params );

	/**
	 * Performs the next available task on the calling thread, returns true if a task was run
	 */
	bool RunNextAvailableTask( );

protected:
	/**
	* Override to register a settings object for this system
	*/
	virtual void CreateEngineSettings( class AXSettingsFile& settings ) override;

private:
	/**
	 * The callback function we supply to the treading system
	 */
	AXThreading::ThreadResult ThreadCallbackFunc( AXThreading::ThreadUserData* userData );

private:
	/**
	* Pointer to the created settings object
	*/
	Settings* mSettings = nullptr;

	/**
	 * Handles to all of our dedicated threads
	 */
	std::vector< AXThreading::ThreadHandle > mDedicatedThreads;

	using TaskCollection = std::list< AXTask* >;

	/**
	 * The tasks we have queued up to run
	 */
	AXMultiReadLockedObject< TaskCollection > mTasks;
};