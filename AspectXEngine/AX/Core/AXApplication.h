// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXSingleton.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Core/AXSystem.h"
#include "AXSettings.h"
#include "AX/Utils/AXParent.h"

class AXApplication : public AXParent< AXSingleton< AXApplication >, AXApplication >
					, public AXImplementsInterface< AXISystemOwner >
{
public:

	struct SetupParams
	{
		AXString mCommandLine;
	};

	class Settings : public AXParent< AXSettingsFile::SettingsItem, Settings >
	{
	public:
		Settings( )
		{
			RegisterProperty( mApplicationName, "Name" );

			RegisterProperty( mMaxFPS, "MaxFps" ).DisplayAsDropDown( { { 0, "Uncapped" }, { 10, "10 fps" }, { 30, "30 fps" }, { 60, "60fps" } } );
		}

	public:
		/**
		 * A name to provide the application with
		 */
		AXProperty< AXString > mApplicationName = "AspectX Application";

		/**
		 * The maximum frame rate the application will run at.
		 */
		AXProperty< uint8_t > mMaxFPS = 60;			// 0 = Uncapped
	};

public:
	/**
	* Constructor
	*/
	AXApplication( );

	/**
	* Destructor
	*/
	~AXApplication( );

	/**
		* Initialises the engine, performs update loop then destroys the engine at the appropriate time
		* and returns out.
		*/
	void RunEngine( const SetupParams& prams );

	/**
		* Calling will result in the engine shutting down
		*/
	void Quit( ) { mQuitEngine = true; }

	/**
	 * Returns the applications settings object
	 */
	Settings& GetSettings( ) { return AXUtils::AssertPtrReturnRef( mAppSettings ); }

private:
	/**
		* Initializes the engine
		*/
	bool Startup( const SetupParams& prams );

	/**
		* Runs the engine loop
		*/
	void RunEngineLoop( );

	/**
		* Shuts down the engine
		*/
	void Shutdown( );

	/**
	 * Creates settings objects for all systems then loads settings
	 */
	void LoadSettings( );

	/**
	 * Passes command line arguments to all systems
	 */
	void HandleCommandLine( const AXString& commandLine );

	/**
	 * Goes through all systems and intiializes them
	 */
	bool InitializeAllSystems( );

	/**
	* Goes through all systems and intiializes them
	*/
	void ShutdownAllSystems( );

protected:

	/**
	 * Creates game systems
	 */
	virtual void CreateDefaultSystems( );

private:

	/**
	* When true the engine will begin shutting down
	*/
	bool mQuitEngine = false;

	/**
	 * Local pointer to the application settings
	 */
	Settings* mAppSettings = nullptr;

	/**
	 * Maintain a pointer to the update ables system so we can tell it to update
	 */
	class AXUpdateables* mUpdatablesSystem = nullptr;
};
