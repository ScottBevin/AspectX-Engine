// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXContentManager_Textures.h"
#include "AX/Content/Importers/AXContentImporter_AXTexture_PNG.h"

AXString AXContentManager< AXTexture >::sContentManagerName = "Textures";

/**
* Constructor
*/
AXContentManager_Textures::AXContentManager_Textures()
{
	RegisterContentImporter< AXContentImporter_AXTexture_PNG >( );
}

/**
* Override to register a settings object for this manager
*/
void AXContentManager_Textures::CreateSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXContentManager_Textures::Settings >( GetName() );
}