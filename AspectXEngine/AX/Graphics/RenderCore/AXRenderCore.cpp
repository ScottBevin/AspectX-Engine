// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXRenderCore.h"
#include "AXGraphicsDevice.h"
#include "AX/Core/AXLogging.h"
#include "AX/Core/AXWindow.h"
#include "AX/Core/AXApplication.h"
#include "AX/Graphics/UI/ImGui/ImGuiColorPicker.h"

// Todo, shouldnt be here...
#include <d3d11.h>

AXString AXSystem< AXRenderCore >::sSystemName = "Render Core";

/**
* Constructor
*/
AXRenderCore::Settings::Settings( )
{
	RegisterProperty( mScreenWidth, "Screen Width" );
	RegisterProperty( mScreenHeight, "Screen Height" );
	RegisterProperty( mFullscreen, "Fullscreen" );
	RegisterProperty( mVsyncEnabled, "VSync Enabled" );
	RegisterProperty( mMultisamples, "Multisamples" ).DisplayAsDropDown( { { 0, "Off" }, { 1, "1x" }, { 2, "2x" }, { 4, "4x" }, { 8, "8x" } } );
	RegisterProperty( mClearColor, "Clear Color" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Constructor
*/
AXRenderCore::AXRenderCore()
{
	
}
	
/**
* Destructor
*/
AXRenderCore::~AXRenderCore()
{
	
}

/**
* Override to register a settings object for this system
*/
void AXRenderCore::CreateEngineSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXRenderCore::Settings >( AXRenderCore::StaticName( ) );
}

/**
* Creates the rendercore
*/
AXSystemBase::InitResult AXRenderCore::OnInitialize()
{
	AXWindow* window( AXWindow::GetFrom( AXApplication::Get( ) ) );

	if( !window )
	{
		return AXSystemBase::InitResult::Failed;
	}

	if( window->GetState( ) != AXSystemBase::State::Initialized )
	{
		return AXSystemBase::InitResult::Retry;
	}

	AXLOG( "RenderCore", "Creating render core" );
	
	AXGraphicsDevice::CreationsParams prams;
	prams.mRenderCoreSettings = mSettings;
	prams.mHwnd = &window->GetWindowHandle( );

	mGraphicsDevice = new AXGraphicsDevice( );

	if( !mGraphicsDevice || !mGraphicsDevice->Create( prams ) )
	{
		return AXSystemBase::InitResult::Failed;
	}

	return AXSystemBase::InitResult::Initialized;
}

/**
* Destroys the rendercore
*/
void AXRenderCore::OnShutdown( )
{
	AXLOG( "RenderCore", "Destroying render core" );

	mGraphicsDevice->Destroy( );
}

/**
* Called at the start of a new frame
*/
void AXRenderCore::BeginFrame( )
{
	// clear the back buffer to a deep blue
	float rgba[4] = { mSettings->mClearColor.Val( ).R(), mSettings->mClearColor.Val( ).G( ), mSettings->mClearColor.Val( ).B( ), 1.0f };
	mGraphicsDevice->GetDXDeviceContext().ClearRenderTargetView( &(mGraphicsDevice->GetBackbuffer()), rgba );
}

/**
* Call to update all updateable objects
*/
void AXRenderCore::Update( float dt )
{
}

/**
* Called at the end of the frame before frame capping
*/
void AXRenderCore::EndFrame( )
{
	mGraphicsDevice->Present( mSettings->mVsyncEnabled );
}