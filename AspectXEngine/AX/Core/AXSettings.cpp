// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXSettings.h"
#include "AXLogging.h"
#include "AX/IO/AXFile.h"
#include "Libs/cJSON/cJSON.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AXApplication.h"
#include "Libs/IMGui/imgui.h"
#include "AX/Content/AXContent.h"

AXString AXSystem< AXSettings >::sSystemName = "Settings";

/**
* Constructor
*/
AXSettings::AXSettings()
	: mEngineSettingsFile( "Engine" )
{

}

/**
* Destructor
*/
AXSettings::~AXSettings()
{

}

/**
* Initialize the system, called after settings are loaded
*/
AXSystemBase::InitResult AXSettings::OnInitialize( )
{ 
	if( AXImGui* imGui = AXImGui::GetFrom( AXApplication::Get( ) ) )
	{
		imGui->RegisterSystemDebugMenuItem( "Window/Settings", std::bind( &AXSettings::ImGuiSettingsWindowCallback, this, std::placeholders::_1 ) );
	}

	return InitResult::Initialized; 
}

bool SettingsItem_ImGuiGetter( void* data, int idx, const char** out_text )
{
	if( out_text )
	{
		*out_text = (( *( static_cast< std::vector< AXSettingsFile::SettingsItem* >* >( data ) ) )[idx])->GetName().c_str();
		return true;
	}

	return false;
}

/**
* Called once a frame to allow systems to update
*/
void AXSettings::Update( float dt )
{
	RenderImGuiSettingsWindow( );
}

/**
* Override to register a settings object for this system
*/
void AXSettings::CreateEngineSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXSettings::Settings >( AXSettings::StaticName( ) );
}

/**
* A callback function to draw the settings window
*/
void AXSettings::ImGuiSettingsWindowCallback( AXImGui::SystemDebugMenuItem& item )
{
	ImGui::MenuItem( item.mText.c_str(), "", &mShouldRenderImGuiSettingsWindow );
}

/**
* Renders the IM gui settings editing window
*/
void AXSettings::RenderImGuiSettingsWindow( )
{
	if( mShouldRenderImGuiSettingsWindow )
	{
		if( ImGui::Begin( "Settings", &mShouldRenderImGuiSettingsWindow ) )
		{
			// Menu

			// Settings classes
			ImGui::BeginChild( "left pane", ImVec2( 150, 0 ), true );

			static uint32_t CurrentlySelectedSettingsPage = 0;

			for( uint32_t i( 0 ); i < mEngineSettingsFile.GetItems().size( ); ++i )
			{
				if( ImGui::Selectable( mEngineSettingsFile.GetItems( )[i]->GetName( ).c_str( ), CurrentlySelectedSettingsPage == i ) )
				{
					CurrentlySelectedSettingsPage = i;
				}
			}

			ImGui::EndChild( );

			ImGui::SameLine( );

			// Properties
			ImGui::BeginGroup( );

			ImGui::BeginChild( "item view", ImVec2( 0, -ImGui::GetItemsLineHeightWithSpacing( ) ) );

			ImGui::Text( mEngineSettingsFile.GetItems( )[CurrentlySelectedSettingsPage]->GetName().c_str() );
			ImGui::Separator( );

			mEngineSettingsFile.GetItems( )[CurrentlySelectedSettingsPage]->RenderPropertiesToImGui( );

			ImGui::EndChild( );

			ImGui::BeginChild( "buttons" );

			if( ImGui::Button( "Reload" ) )
			{
				mEngineSettingsFile.Load( );
			}

			ImGui::SameLine( );

			if( ImGui::Button( "Save" ) ) 
			{ 
				mEngineSettingsFile.Save( );
			}

			ImGui::EndChild( );

			ImGui::EndGroup( );

			ImGui::End( );
		}
	}
}

/**
* Takes the filename and returns a valid path to the settings file
*/
AXString AXSettings::BuildPathToConfigFile( const AXString& filename )
{
	AXString contentRootPath = ".";

	if( AXContent* content = AXContent::GetFrom( AXApplication::Get( ) ) )
	{
		contentRootPath = content->GetSettings( ).mContentRootDirectory.Val();
	}

	if( AXSettings* settings = AXSettings::GetFrom( AXApplication::Get( ) ) )
	{
		return AXUtils::FormatString( "%s/%s/%s.%s",
			contentRootPath.c_str( ),
			settings->GetSettings().mConfigDirectory->c_str( ),
			filename.c_str( ),
			settings->GetSettings( ).mConfigExtention->c_str( ) );
	}

	return contentRootPath;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AXSettingsFile::AXSettingsFile( const AXString& name )
	: mName( name )
{

}

/**
* Attempts to load the settings from the settings file if one exists
*/
void AXSettingsFile::Load( )
{
	AXFile file( AXSettings::BuildPathToConfigFile( mName ), AXFile::FileOpenMode::Read, AXFile::DataMode::Normal );

	if( file.IsOpen( ) )
	{
		AXFile::InternalFileBuffer& buffer( file.ReadFileToInternalBuffer( ) );

		if( buffer.mBuffer )
		{
			if( cJSON* jsonRoot = cJSON_Parse( (const char*)buffer.mBuffer ) )
			{
				if( cJSON* meta = cJSON_GetObjectItem( jsonRoot, "Meta" ) )
				{
					if( cJSON* versionJSON = cJSON_GetObjectItem( meta, "Version" ) )
					{
						int version = versionJSON->valueint;
					}
				}

				if( cJSON* systemsRoot = cJSON_GetObjectItem( jsonRoot, "Systems" ) )
				{
					for( SettingsItem* item : mSettingsItems )
					{
						if( cJSON* itemRoot = cJSON_GetObjectItem( systemsRoot, item->GetName( ).c_str( ) ) )
						{
							item->ReadPropertiesFromJSON( *itemRoot );
						}
					}
				}

				cJSON_Delete( jsonRoot );
			}
		}

		file.DestroyInternalBuffer( );
		file.CloseFile( );
	}
}

/**
* Attempts to save the settings from a settings file if one exists
*/
void AXSettingsFile::Save( )
{
	AXFile file( AXSettings::BuildPathToConfigFile( mName ), AXFile::FileOpenMode::Write, AXFile::DataMode::Normal );

	if( file.IsOpen( ) )
	{
		if( cJSON* jsonRoot = cJSON_CreateObject( ) )
		{
			if( cJSON* meta = cJSON_CreateObject( ) )
			{
				cJSON_AddItemToObject( jsonRoot, "Meta", meta );

				cJSON_AddNumberToObject( meta, "Version", 0 );
			}

			if( cJSON* systemsRoot = cJSON_CreateObject( ) )
			{
				cJSON_AddItemToObject( jsonRoot, "Systems", systemsRoot );

				for( SettingsItem* item : mSettingsItems )
				{
					if( cJSON* itemRoot = cJSON_CreateObject( ) )
					{
						cJSON_AddItemToObject( systemsRoot, item->GetName( ).c_str( ), itemRoot );

						item->WritePropertiesToJSON( *itemRoot );
					}
				}
			}

			if( char* buffer = cJSON_Print( jsonRoot ) )
			{
				file.CreateInternalBuffer( strlen( buffer ), (uint8_t*)buffer );
				file.WriteInternalBufferToFile( );
			}

			cJSON_Delete( jsonRoot );
		}

		file.DestroyInternalBuffer( );
		file.CloseFile( );
	}
}
