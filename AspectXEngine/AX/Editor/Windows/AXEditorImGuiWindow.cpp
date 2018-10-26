// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXEditorImGuiWindow.h"

void AXEditorImGuiWindow::Render( )
{
	if( mVisible )
	{
		if( ImGui::Begin( mWindowTitle.c_str( ), &mVisible ) )
		{
			RenderContents( );
		}

		ImGui::End( );
	}
}

/**
* Callback into IMGui to toggle visibility of this window
*/
void AXEditorImGuiWindow::ToggleVisibilityMenuOption( AXImGui::SystemDebugMenuItem& item )
{
	ImGui::MenuItem( item.mText.c_str( ), "", &mVisible );
}