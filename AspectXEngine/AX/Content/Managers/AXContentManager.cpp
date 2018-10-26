// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXContentManager.h"
#include "AX/Content/AXContent.h"
#include "AX/Core/AXApplication.h"
#include "AX/Content/Importers/AXContentImporter.h"

/**
* Initialises a new content importer
*/
void AXContentManagerBase::InitialiseNewImporter( AXContentImporterBase& importer )
{
	if( AXContent* content = AXContent::GetFrom( AXApplication::Get( ) ) )
	{
		importer.CreateSettings( content->GetContentSettingsFile( ) );
	}
}