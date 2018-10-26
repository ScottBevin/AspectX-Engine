// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXContentImporter_AXTexture_PNG.h"
#include "AXContentImporter.h"

AXString AXContentImporter< AXTexture >::sSupportedExtention = "PNG";

/**
* Derived types should implement this function
*/
AXTexture* AXContentImporter_AXTexture_PNG::ImportAsset( const AXFile::InternalFileBuffer& fileBuffer )
{
	return nullptr;
}

/**
* Override to register a settings object for this importer
*/
void AXContentImporter_AXTexture_PNG::CreateSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXContentImporter_AXTexture_PNG::Settings >( sSupportedExtention );
}

/**
* Override to return a pointer to a valid settings item if one exists
*/
AXSettingsFile::SettingsItem* AXContentImporter_AXTexture_PNG::GetSettings( )
{
	return mSettings;
}