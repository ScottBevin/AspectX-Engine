// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AX/IO/AXDirectory.h"

class AXEditorImGuiWindow : public AXParent< AXBaseObject, AXEditorImGuiWindow >
{
public:
	AXEditorImGuiWindow( const AXString& windowName ) :
		mWindowTitle ( windowName ) { }

	const AXString& GetTitle( ) const { return mWindowTitle; }

	void Render( );

	/**
	 * Callback into IMGui to toggle visibility of this window
	 */
	void ToggleVisibilityMenuOption( AXImGui::SystemDebugMenuItem& item );

protected:

	/**
	 * Override to render the contents of this editor window
	 */
	virtual void RenderContents( ) = 0;

private:
	/**
	 * The name given to this editor window
	 */
	AXString mWindowTitle;

	/**
	 * Is this window currently visible
	 */
	bool mVisible = true;
};