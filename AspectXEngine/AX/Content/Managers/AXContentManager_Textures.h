// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Content/Managers/AXContentManager.h"
#include "AX/Utils/AXParent.h"
#include "AX/Content/Types/AXTexture.h"

class AXContentManager_Textures : public AXParent< AXContentManager< AXTexture >, AXContentManager_Textures >
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
			RegisterProperty( mBuiltFileExt, "Built file extension" );
		}

	public:
		AXProperty< AXString > mBuiltFileExt = ".tex";
	};

public:
	/**
	* Constructor
	*/
	AXContentManager_Textures();

	/**
	* Override to register a settings object for this manager
	*/
	virtual void CreateSettings( class AXSettingsFile& settings ) override;

	/**
	* Override to return a pointer to a valid settings item if one exists
	*/
	virtual AXSettingsFile::SettingsItem* GetSettings( ) override { return mSettings; }

private:
	Settings* mSettings = nullptr;
};