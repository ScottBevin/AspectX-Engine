// Copyright 2016 Scott Bevin, All Rights Reserved

#include <Windows.h>

#include "AXWindow.h"
#include "AXApplication.h"
#include "AXLogging.h"

#include "AX/Utils/AXUtils.h"

#include "AX/Graphics/RenderCore/AXRenderCore.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"

AXString AXSystem< AXWindow >::sSystemName = "Window";

/**
* Constructor
*/
AXWindow::AXWindow()
{
	
}
	
/**
* Destructor
*/
AXWindow::~AXWindow()
{
	
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	if (auto imGuiSys = AXImGui::GetFrom(AXApplication::Get()))
	{
		if (imGuiSys->HandleWndProc(umsg, wparam, lparam))
		{
			return true;
		}
	}

	switch (umsg)
	{
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
	{
		AXApplication::Get().Quit();
		return true;

	} break;
	}

	return DefWindowProc(hwnd, umsg, wparam, lparam);;
}

/**
* Initialize the system, called after settings are loaded
*/
AXSystemBase::InitResult AXWindow::OnInitialize()
{
	AXLOG( "Window", "Creating window" );

	WNDCLASSEX wc;
	AXUtils::ZeroStruct( wc );

	mHInstance = GetModuleHandle( nullptr );

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHInstance;
	wc.hIcon = LoadIcon( nullptr, IDI_WINLOGO );
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wc.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = AXApplication::Get( ).GetSettings().mApplicationName.Val().c_str();
	wc.cbSize = sizeof( WNDCLASSEX );

	if( !RegisterClassEx( &wc ) )
	{
		AXERROR( "Window", "Failed to register window." );
		return AXSystemBase::InitResult::Failed;
	}

	unsigned int width( 800 );
	unsigned int height( 600 );

	if( AXRenderCore* renderCore = AXRenderCore::GetFrom( AXApplication::Get( ) ) )
	{
		AXRenderCore::Settings& settings( renderCore->GetSettings( ) );

		width = settings.mScreenWidth;
		height = settings.mScreenHeight;
	}

	mHwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		wc.lpszClassName, wc.lpszClassName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		mHInstance,
		NULL );

	if( mHwnd == nullptr )
	{
		AXERROR( "Window", "Failed to create window." );
		return AXSystemBase::InitResult::Failed;
	}

	ShowWindow( mHwnd, SW_SHOW );
	SetForegroundWindow( mHwnd );
	SetFocus( mHwnd );
	ShowCursor( true );

	return AXSystemBase::InitResult::Initialized;
}

/**
* Shutdown the system
*/
void AXWindow::OnShutdown( )
{
	AXLOG( "Window", "Destroying window" );

	ShowCursor( true );

	DestroyWindow( mHwnd );
	UnregisterClass( "AspectX Engine", mHInstance );
}

/**
* Runs all windows messages
*/
void AXWindow::ProcessMessages( )
{
	MSG msg;
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

/**
* Call to update all updateable objects
*/
void AXWindow::Update( float dt )
{
	ProcessMessages( );
}
