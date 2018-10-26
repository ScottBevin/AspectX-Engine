// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dCompiler.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include "AXRenderCore.h"
#include "AXGraphicsDevice.h"
#include "AX/Core/AXLogging.h"

/**
* Constructor
*/
AXGraphicsDevice::AXGraphicsDevice()
{

}

/**
* Destructor
*/
AXGraphicsDevice::~AXGraphicsDevice()
{

}

/**
* Creates the graphics device, returning a success flag
*/
bool AXGraphicsDevice::Create( const CreationsParams& prams )
{
	/*
	HRESULT result;

	IDXGIFactory* factory;
	result = CreateDXGIFactory( __uuidof( IDXGIFactory ), ( void** )&factory );
	if( FAILED( result ) )
	{
		AXERROR( "GDevice", "Failed to create IDCGIFactory. Error: %d", result );
		return false;
	}

	IDXGIAdapter* adapter;
	result = factory->EnumAdapters( 0, &adapter );
	if( FAILED( result ) )
	{
		AXERROR( "GDevice", "Failed to enumerate adaters. Error: %d", result );
		return false;
	}

	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs( 0, &adapterOutput );
	if( FAILED( result ) )
	{
		AXERROR( "GDevice", "Failed to output adaters. Error: %d", result );
		return false;
	}

	UINT numModes = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	result = adapterOutput->GetDisplayModeList( format, 0, &numModes, nullptr );
	if( FAILED( result ) )
	{
		AXERROR( "GDevice", "Failed to count display modes. Error: %d", result );
		return false;
	}

	DXGI_MODE_DESC* displayModes = new DXGI_MODE_DESC[ numModes ];

	result = adapterOutput->GetDisplayModeList( format, 0, &numModes, displayModes );
	if( FAILED( result ) )
	{
		AXERROR( "GDevice", "Failed to get display modes. Error: %d", result );
		return false;
	}

	delete[] displayModes;
	adapterOutput->Release( );
	adapter->Release( );
	factory->Release( );

	*/

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory( &scd, sizeof( DXGI_SWAP_CHAIN_DESC ) );

	scd.BufferCount = 1;                                    
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      
	scd.OutputWindow = (HWND)(prams.mHwnd);                                
	scd.SampleDesc.Count = prams.mRenderCoreSettings->mMultisamples;                               
	scd.Windowed = TRUE;                                    

	HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&mSwapChain,
		&mDXDevice,
		NULL,
		&mDXDeviceContext );

	if( FAILED( hr ) )
	{
		AXERROR( "GDevice", "Failed to create graphics device. Error: %d", hr );
		return false;
	}

	// Temp stuff setting up a backbuffer render target

	// get the address of the back buffer
	ID3D11Texture2D *backBuffer;
	mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBuffer );

	// use the back buffer address to create the render target
	mDXDevice->CreateRenderTargetView( backBuffer, NULL, &mBackbuffer );
	backBuffer->Release( );

	// set the render target as the back buffer
	mDXDeviceContext->OMSetRenderTargets( 1, &mBackbuffer, NULL );

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;

	mDXDeviceContext->RSSetViewports( 1, &viewport );

	return true;
}

/**
* Destroys the graphics device
*/
void AXGraphicsDevice::Destroy( )
{
	mSwapChain->Release( );
	mDXDevice->Release( );
	mDXDeviceContext->Release( );
}

/**
* Present at the end of a frame
*/
void AXGraphicsDevice::Present( bool vsync )
{
	// switch the back buffer and the front buffer
	mSwapChain->Present( vsync ? 1 : 0, 0 );
}