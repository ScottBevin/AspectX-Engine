// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXProperties.h"
#include "AXUtils.h"

/**
* Destructor
*/
AXIPropertyHolder::~AXIPropertyHolder( )
{
	mProperties.clear( );
}

/**
* Writes all properties into a JSON structure
*/
void AXIPropertyHolder::WritePropertiesToJSON( cJSON& jsonRoot ) const
{
	if( cJSON* propRoot = cJSON_CreateObject( ) )
	{
		cJSON_AddItemToObject( &jsonRoot, "Properties", propRoot );

		for( const AXPropertyBase* prop : mProperties )
		{
			prop->WriteToJSON( *propRoot );
		}
	}
}

/**
* Reads all the properties out of a JSON structure
*/
void AXIPropertyHolder::ReadPropertiesFromJSON( cJSON& jsonRoot )
{
	if( cJSON* propRoot = cJSON_GetObjectItem( &jsonRoot, "Properties" ) )
	{
		for( AXPropertyBase* prop : mProperties )
		{
			prop->ReadFromJSON( *propRoot );
		}
	}
}


/**
* Renders all appropriate properties into the current ImGuiContext
*/
void AXIPropertyHolder::RenderPropertiesToImGui( )
{
	ImGui::Columns( 2 );

	for( uint32_t i( 0 ); i < mProperties.size( ); ++i )
	{
		ImGui::PushID( i );

		ImGui::Text( mProperties[ i ]->GetName( ).c_str( ) );

		ImGui::NextColumn( );

		mProperties[ i ]->RenderToImGui( );

		ImGui::NextColumn( );

		ImGui::PopID( );
	}

	ImGui::Columns( 1 );
}