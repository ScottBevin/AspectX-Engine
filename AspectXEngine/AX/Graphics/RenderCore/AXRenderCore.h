// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Core/AXSystem.h"
#include "AX/Core/AXSettings.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Utils/AXProperties.h"
#include "AX/Utils/AXColor.h"
#include "AX/Utils/AXParent.h"

class AXRenderCore : public AXParent< AXSystem< AXRenderCore >, AXRenderCore >
{
public:
	class Settings : public AXParent< AXSettingsFile::SettingsItem, Settings >
	{
	public:

		/**
		 * Constructor
		 */
		Settings( );

	public:
		AXProperty< unsigned int > mScreenWidth = 1024;

		AXProperty< unsigned int >  mScreenHeight = 768;

		AXProperty< bool > mFullscreen = false;

		AXProperty< bool > mVsyncEnabled = true;

		AXProperty< uint8_t > mMultisamples = 4;

		AXProperty< AXColorRGBf > mClearColor = AXColorRGBf( 0.0f, 0.2f, 0.4f );
	};

public:
	/**
	* Constructor
	*/
	AXRenderCore();
	
	/**
	* Destructor
	*/
	~AXRenderCore();
	
	Settings& GetSettings( ) { return AXUtils::AssertPtrReturnRef( mSettings ); }

	class AXGraphicsDevice& GetGraphicsDevice( ) { return AXUtils::AssertPtrReturnRef( mGraphicsDevice ); }

protected:
	/**
	* Override to register a settings object for this system
	*/
	virtual void CreateEngineSettings( class AXSettingsFile& settings ) override;

	/**
	* Creates the rendercore
	*/
	InitResult OnInitialize( ) override;

	/**
	* Destroys the rendercore
	*/
	void OnShutdown( ) override;

	/**
	* Called at the start of a new frame
	*/
	virtual void BeginFrame( ) override;

	/**
	* Call to update all updateable objects
	*/
	virtual void Update( float dt ) override;

	/**
	* Called at the end of the frame before frame capping
	*/
	virtual void EndFrame( ) override;

private:

	/**
	 * Pointer to the created settings object
	 */
	Settings* mSettings = nullptr;

	/**
	 * Pointer to our graphics device
	 */
	class AXGraphicsDevice* mGraphicsDevice = nullptr;
};
