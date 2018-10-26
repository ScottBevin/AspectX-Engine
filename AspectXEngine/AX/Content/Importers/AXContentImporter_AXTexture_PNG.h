// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AXContentImporter.h"
#include "AX/Content/Types/AXTexture.h"

class AXContentImporter_AXTexture_PNG : public AXParent< AXContentImporter< AXTexture >, AXContentImporter_AXTexture_PNG >
{
private:
	class Settings : public AXParent< AXSettingsFile::SettingsItem, Settings >
	{
	public:

		/**
		* Constructor
		*/
		Settings( )
		{
			RegisterProperty( mTestProp1, "Test prop 1" );
			RegisterProperty( mTestProp2, "Test prop 2" );
		}

	public:
		AXProperty< AXString > mTestProp1 = "Content";
		AXProperty< bool > mTestProp2 = false;
	};

protected:
	/**
	* Derived types should implement this function
	*/
	virtual AXTexture* ImportAsset( const AXFile::InternalFileBuffer& fileBuffer ) override;

	/**
	* Override to register a settings object for this importer
	*/
	virtual void CreateSettings( class AXSettingsFile& settings ) override;

	/**
	* Override to return a pointer to a valid settings item if one exists
	*/
	virtual AXSettingsFile::SettingsItem* GetSettings( ) override;

private:
	Settings* mSettings = nullptr;
};