// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AXRenderCore.h"
#include "AX/Utils/AXBaseObject.h"

class AXGraphicsDevice : public AXBaseObject
{
public:
	struct CreationsParams
	{
		AXRenderCore::Settings* mRenderCoreSettings;
		struct HWND__* mHwnd;
	};

public:
	/**
	* Constructor
	*/
	AXGraphicsDevice();

	/**
	* Destructor
	*/
	~AXGraphicsDevice();
	
	/**
	 * Creates the graphics device, returning a success flag
	 */
	bool Create( const CreationsParams& prams );

	/**
	 * Destroys the graphics device
	 */
	void Destroy( );

	/**
	 * Present at the end of a frame
	 */
	void Present( bool vsync );

	struct ID3D11Device& GetDXDevice( ) { return AXUtils::AssertPtrReturnRef( mDXDevice ); }
	struct ID3D11DeviceContext& GetDXDeviceContext( ) { return AXUtils::AssertPtrReturnRef( mDXDeviceContext ); }
	struct ID3D11RenderTargetView& GetBackbuffer( ) { return AXUtils::AssertPtrReturnRef( mBackbuffer ); }

private:
	/**
	* Pointer to the direct X device
	*/
	struct ID3D11Device* mDXDevice = nullptr;

	/**
	* Pointer to the direct x device context
	*/
	struct ID3D11DeviceContext* mDXDeviceContext = nullptr;

	/**
	* Pointer to the direct x swap chain
	*/
	struct IDXGISwapChain* mSwapChain = nullptr;

	/**
	 * A temp backbuffer obj
	 */
	struct ID3D11RenderTargetView* mBackbuffer = nullptr;
};