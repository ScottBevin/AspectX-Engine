// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXEditor.h"
#include "Windows/AXContentBrowserImGuiWindow.h"
#include "AX/Core/AXApplication.h"

AXString AXSystem< AXEditor >::sSystemName = "Editor";

/**
* Initialise the system, called after settings are loaded
*/
AXEditor::InitResult AXEditor::OnInitialize( )
{
	RegisterWindow( new AXContentBrowserImGuiWindow( ) );

	return AXEditor::InitResult::Initialized;
} 

/**
* Called once a frame to allow systems to update
*/
void AXEditor::Update( float dt )
{
	for( AXEditorImGuiWindow* window : mWindows )
	{
		window->Render( );
	}
}

/**
* Override to register a settings object for this system
*/
void AXEditor::CreateEngineSettings( class AXSettingsFile& settings )
{
	mSettings = settings.RegisterNewItem< AXEditor::Settings >( AXEditor::StaticName( ) );
}

/**
* Registers a new editor window and does all the various setup
*/
void AXEditor::RegisterWindow( AXEditorImGuiWindow* newWindow )
{
	mWindows.push_back( newWindow );

	if( AXImGui* imGui = AXImGui::GetFrom( AXApplication::Get( ) ) )
	{
		imGui->RegisterSystemDebugMenuItem( AXString( "Editor/Windows/" ) + newWindow->GetTitle(), 
			std::bind( &AXEditorImGuiWindow::ToggleVisibilityMenuOption, newWindow, std::placeholders::_1 ) );
	}
}