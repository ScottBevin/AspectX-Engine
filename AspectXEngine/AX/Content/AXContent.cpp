// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXContent.h"

#include "Managers/AXContentManager_Textures.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AX/Core/AXApplication.h"
#include "AX/IO/AXDirectory.h"
#include "Importers/AXContentImporter.h"

AXString AXSystem< AXContent >::sSystemName = "Content";

/**
* Initialise the system, called after settings are loaded
*/
AXContent::InitResult AXContent::OnInitialize( )
{
	RegisterContentManager< AXContentManager_Textures >( );

	mContentSettingsFile.Load( );

	if( AXImGui* imGui = AXImGui::GetFrom( AXApplication::Get( ) ) )
	{
		imGui->RegisterSystemDebugMenuItem( "Window/Content Managers", std::bind( &AXContent::ImGuiContentManagersWindowCallback, this, std::placeholders::_1 ) );
	}

	return AXContent::InitResult::Initialized;
}


/**
* Called once a frame to allow systems to update
*/
void AXContent::Update( float dt )
{
	RenderContentManagersImGuiWindow( );
}

/**
* Shutdown the system
*/
void AXContent::OnShutdown( )
{

}

/**
* Attempts to find and return a content manager by name, returns null if it cannot be found
*/
const AXContentManagerBase* AXContent::FindContentManagerByType( const std::string& name ) const
{
	auto it( mContentManagers.find( name ) );

	if( it != mContentManagers.end( ) )
	{
		return it->second;
	}

	return nullptr;
}

/**
* Attempts to find and return a content manager by supported extension, returns null if it cannot be found
*/
const AXContentManagerBase* AXContent::FindContentManagerByExtension( const std::string& ext ) const
{
	for( auto it : mContentManagers )
	{
		if( it.second->IsExtentionSupported( ext ) )
		{
			return it.second;
		}
	}

	return nullptr;
}

/**
* Will attempt to find a valid manager to load the specific asset then queue it for loading, returns a handle
* which can be inspected for validity / load state
*/
AXAssetHandleBase AXContent::RequestAssetLoad( const std::string& name ) const
{
	if( const AXContentManagerBase* manager = FindContentManagerByExtension( AXFile::GetExtention( name ) ) )
	{

	}

	return AXAssetHandleBase( );
}

/**
* Override to register a settings object for this system
*/
void AXContent::CreateEngineSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXContent::Settings >( AXContent::StaticName( ) );
}

/**
* A callback function to draw the content debug window
*/
void AXContent::ImGuiContentManagersWindowCallback( AXImGui::SystemDebugMenuItem& item )
{
	bool wasVisible( mShouldRenderImGuiContentManagersWindow );
	ImGui::MenuItem( item.mText.c_str( ), "", &mShouldRenderImGuiContentManagersWindow );

	if( wasVisible && !mShouldRenderImGuiContentManagersWindow )
	{
		mContentSettingsFile.Save( );
	}
}

/**
* Renders the analysis window in imgui
*/
void AXContent::RenderContentManagersImGuiWindow( )
{
	if( mShouldRenderImGuiContentManagersWindow )
	{
		if( ImGui::Begin( "Content Managers", &mShouldRenderImGuiContentManagersWindow ) )
		{
			// Menu

			// Settings classes
			ImGui::BeginChild( "left pane", ImVec2( 150, 0 ), true );

			static uint32_t CurrentlySelectedManagerPage = 0;
			AXContentManagerBase* currentlySelectedContentManager( nullptr );

			int i( 0 );
			for( auto contentManager : mContentManagers )
			{
				if( ImGui::Selectable( contentManager.second->GetName( ).c_str( ), CurrentlySelectedManagerPage == i ) )
				{
					CurrentlySelectedManagerPage = i;
				}

				if( CurrentlySelectedManagerPage == i )
				{
					currentlySelectedContentManager = contentManager.second;
				}

				++i;
			}

			ImGui::EndChild( );

			ImGui::SameLine( );

			// Properties
			ImGui::BeginGroup( );

			ImGui::BeginChild( "item view", ImVec2( 0, 0 ) );

			RenderContentManagerDetailsImGuiWindow( *currentlySelectedContentManager );

			ImGui::EndChild( );

			ImGui::EndGroup( );
		}

		ImGui::End( );

		if( !mShouldRenderImGuiContentManagersWindow )
		{
			mContentSettingsFile.Save( );
		}
	}
}

/**
* Renders the analysis window in imgui
*/
void AXContent::RenderContentManagerDetailsImGuiWindow( AXContentManagerBase& manager )
{
	ImGui::Text( manager.GetName( ).c_str( ) );
	ImGui::Separator( );

	// General settings

	if( AXSettingsFile::SettingsItem* settings = manager.GetSettings( ) )
	{
		settings->RenderPropertiesToImGui( );
	}

	// Importers
	
	if( ImGui::CollapsingHeader( "Content Importers" ) )
	{
		int i( 0 );

		for( auto contentImporter : manager.GetImporters( ) )
		{
			ImGui::PushID( i++ );

			if( ImGui::CollapsingHeader( contentImporter.second->GetSupportedExtention().c_str() ) )
			{
				if( AXSettingsFile::SettingsItem* settings = contentImporter.second->GetSettings( ) )
				{
					settings->RenderPropertiesToImGui( );
				}
			}

			ImGui::PopID( );
		}
	}
}