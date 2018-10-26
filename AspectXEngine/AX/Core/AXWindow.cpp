// Copyright 2016 Scott Bevin, All Rights Reserved

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "AXWindow.h"
#include "AXApplication.h"
#include "AXLogging.h"

#include "AX/Utils/AXUtils.h"

#include "AX/Graphics/RenderCore/AXRenderCore.h"
#include "Libs/IMGui/imgui.h"

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

LRESULT CALLBACK WndProc( HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam )
{
	// Todo: Full of ImGui hacks
	ImGuiIO& io = ImGui::GetIO( );

	switch( umsg )
	{
		case WM_LBUTTONDOWN:
			io.MouseDown[0] = true;
			return true;
		case WM_LBUTTONUP:
			io.MouseDown[0] = false;
			return true;
		case WM_RBUTTONDOWN:
			io.MouseDown[1] = true;
			return true;
		case WM_RBUTTONUP:
			io.MouseDown[1] = false;
			return true;
		case WM_MBUTTONDOWN:
			io.MouseDown[2] = true;
			return true;
		case WM_MBUTTONUP:
			io.MouseDown[2] = false;
			return true;
		case WM_MOUSEWHEEL:
			io.MouseWheel += GET_WHEEL_DELTA_WPARAM( wparam ) > 0 ? +1.0f : -1.0f;
			return true;
		case WM_MOUSEMOVE:
			io.MousePos.x = ( signed short )( lparam );
			io.MousePos.y = ( signed short )( lparam >> 16 );
			return true;
		case WM_KEYDOWN:
			if( wparam < 256 )
				io.KeysDown[wparam] = 1;
			return true;
		case WM_KEYUP:
			if( wparam < 256 )
				io.KeysDown[wparam] = 0;
			return true;
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			if( wparam > 0 && wparam < 0x10000 )
				io.AddInputCharacter( ( unsigned short )wparam );
			return true;

		case WM_QUIT:
		case WM_CLOSE:
		case WM_DESTROY:
		{
			AXApplication::Get( ).Quit( );
			return true;

		} break;

		default:
		{
			return DefWindowProc( hwnd, umsg, wparam, lparam );
		}
	}

	return 0;
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
