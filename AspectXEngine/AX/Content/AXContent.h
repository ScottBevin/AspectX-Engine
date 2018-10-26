// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Core/AXSystem.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AX/Core/AXSettings.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Core/AXLogging.h"
#include "AX/Content/Managers/AXContentManager.h"
#include "AXAssetHandle.h"

class AXContent : public AXParent< AXSystem< AXContent >, AXContent >
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
			RegisterProperty( mContentRootDirectory, "Root Directory" );
		}

	public:
		AXProperty< AXString > mContentRootDirectory = "Content";
	};

public:
	
	Settings& GetSettings( ) { return AXUtils::AssertPtrReturnRef( mSettings ); }

	template< class T >
	void RegisterContentManager( );

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
	 * Returns the contents settings file
	 */
	AXSettingsFile& GetContentSettingsFile( ) { return mContentSettingsFile; }

	/**
	 * Attempts to find and return a content manager by type, returns null if it cannot be found
	 */
	template< class T >
	const T* FindContentManagerByType( ) const;

	/**
	 * Attempts to find and return a content manager by name, returns null if it cannot be found
	 */
	const AXContentManagerBase* FindContentManagerByType( const std::string& name ) const;

	/**
	* Attempts to find and return a content manager by supported extension, returns null if it cannot be found
	*/
	const AXContentManagerBase* FindContentManagerByExtension( const std::string& ext ) const;

	/**
	 * Will attempt to find a valid manager to load the specific asset then queue it for loading, returns a handle
	 * which can be inspected for validity / load state
	 */
	AXAssetHandleBase RequestAssetLoad( const std::string& name ) const;

	/**
	* Will attempt to find a valid manager that can load this type of asset, will validate the path and then attempt to load.returns a handle
	* which can be inspected for validity / load state
	*/
	template< class T >
	typename T::Handle RequestAssetLoad( const std::string& name ) const;

protected:
	/**
	* Override to register a settings object for this system
	*/
	virtual void CreateEngineSettings( class AXSettingsFile& settings ) override;

private:
	/**
	* A callback function to draw the content debug window
	*/
	void ImGuiContentManagersWindowCallback( AXImGui::SystemDebugMenuItem& item );

	/**
	 * Renders the analysis window in imgui
	 */
	void RenderContentManagersImGuiWindow( );

	/**
	* Renders the analysis window in imgui
	*/
	void RenderContentManagerDetailsImGuiWindow( AXContentManagerBase& manager );

private:
	/**
	* Pointer to the created settings object
	*/
	Settings* mSettings = nullptr;

	/**
	 * The content managers currently registered
	 */
	std::map< AXString, AXContentManagerBase* > mContentManagers;

	/**
	 * A settings file for storing content settings in
	 */
	AXSettingsFile mContentSettingsFile = AXSettingsFile( "Content" );

	/**
	* If true the settings ImGui window will render
	*/
	bool mShouldRenderImGuiContentManagersWindow = false;
};

/**
* Attempts to find and return a content manager by type, returns null if it cannot be found
*/
template< class T >
const T* AXContent::FindContentManagerByType( ) const
{
	static_assert( std::is_base_of< AXContentManagerBase, T >::value, "Must be a content manager" );

	return static_cast< T* >( FindContentManagerByType( T::Name( ) ) );
}

template< class T >
void AXContent::RegisterContentManager( )
{
	static_assert( std::is_base_of< AXContentManagerBase, T >::value, "Must be a content manager" );

	if( mContentManagers.find( T::Name( ) ) == mContentManagers.end( ) )
	{
		mContentManagers[T::Name( )] = new T( );
		mContentManagers[T::Name( )]->CreateSettings( mContentSettingsFile );

		AXLOG( "Content", "Registering content manager: %s.", T::Name( ) );
	}
}