// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Core/AXSystem.h"
#include "AX/Utils/AXProperties.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AX/Utils/AXInterface.h"
#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"
#include "AX/Utils/AXUtils.h"

/**
* A wrapper around a settings file
*/
class AXSettingsFile : public AXParent< AXBaseObject, AXSettingsFile >
{
public:
	/**
	* A wrapper around a settings object
	*/
	class SettingsItem : public AXParent< AXBaseObject, SettingsItem >
		, public AXImplementsInterface< AXIPropertyHolder >
	{
	public:
		const AXString& GetName( ) const { return mName; }

		friend class AXSettingsFile;

	private:
		AXString mName = "Undefined";
	};

public:
	AXSettingsFile( const AXString& name );

	/**
	* Registers a new item into the settings system, returns a pointer to the item
	* if the same settings item gets registered twice, will return a pointer to the
	* original.
	*/
	template< class T >
	T* RegisterNewItem( const AXString& name );

	/**
	* Attempts to load the settings from the settings file if one exists
	*/
	void Load( );

	/**
	* Attempts to save the settings from a settings file if one exists
	*/
	void Save( );

	const AXString& GetName( ) const { return mName; }
	
	const std::vector< SettingsItem* >& GetItems( ) const { return mSettingsItems; }

private:
	AXString mName = "Settings";

	/**
	* The container for all registered items
	*/
	std::vector< SettingsItem* > mSettingsItems;
};

class AXSettings : public AXParent< AXSystem< AXSettings >, AXSettings >
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
			RegisterProperty( mConfigDirectory, "Config Directory" );
			RegisterProperty( mConfigExtention, "Config Extention" );
		}

	public:
		AXProperty< AXString > mConfigDirectory = "Config";
		AXProperty< AXString > mConfigExtention = "cfg";

	};

public:
	/**
	* Constructor
	*/
	AXSettings();
	
	/**
	* Destructor
	*/
	~AXSettings();
	
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
	 * Takes the filename and returns a valid path to the settings file
	 */
	static AXString BuildPathToConfigFile( const AXString& filename );

	/**
	 * Returns a reference to the engine settings file
	 */
	AXSettingsFile& GetEngineSettingsFile( ) { return mEngineSettingsFile; }

protected:
	/**
	* Override to register a settings object for this system
	*/
	virtual void CreateEngineSettings( class AXSettingsFile& settings ) override;

private:

	/**
	 * A callback function to draw the settings window
	 */
	void ImGuiSettingsWindowCallback( AXImGui::SystemDebugMenuItem& item );

	/**
	 * Renders the IM gui settings editing window
	 */
	void RenderImGuiSettingsWindow( );

private:
	/**
	* Pointer to the created settings object
	*/
	Settings* mSettings = nullptr;

	/**
	 * The default settings file
	 */
	AXSettingsFile mEngineSettingsFile;

	/**
	 * If true the settings ImGui window will render
	 */
	bool mShouldRenderImGuiSettingsWindow = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Registers a new item into the settings system, returns a pointer to the item
* if the same settings item gets registered twice, will return a pointer to the
* original.
*/
template< class T >
T* AXSettingsFile::RegisterNewItem( const AXString& name )
{
	T newItem;

	for( SettingsItem* item : mSettingsItems )
	{
		if( item->mName == newItem.GetName( ) )
		{
			return static_cast< T* >( item );
		}
	}

	T* newItem2( new T( ) );
	newItem2->mName = name;

	mSettingsItems.push_back( newItem2 );

	return newItem2;
}