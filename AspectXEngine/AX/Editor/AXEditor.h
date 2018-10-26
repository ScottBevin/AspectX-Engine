// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Core/AXSystem.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Core/AXSettings.h"

class AXEditor : public AXParent< AXSystem< AXEditor >, AXEditor >
{
	class Settings : public AXParent< AXSettingsFile::SettingsItem, Settings >
	{
	public:

		/**
		* Constructor
		*/
		Settings( )
		{
		}

	public:
	};

public:
	
	Settings& GetSettings( ) { return AXUtils::AssertPtrReturnRef( mSettings ); }

	/**
	* Initialise the system, called after settings are loaded
	*/
	virtual InitResult OnInitialize( ) override;

	/**
	* Called once a frame to allow systems to update
	*/
	virtual void Update( float dt ) override;

protected:
	/**
	* Override to register a settings object for this system
	*/
	virtual void CreateEngineSettings( class AXSettingsFile& settings ) override;

private:
	/**
	 * Registers a new editor window and does all the various setup
	 */
	void RegisterWindow( class AXEditorImGuiWindow* newWindow );

private:
	/**
	* Pointer to the created settings object
	*/
	Settings* mSettings = nullptr;

	/**
	 * The collection of editor windows
	 */
	std::vector< class AXEditorImGuiWindow* > mWindows;
};