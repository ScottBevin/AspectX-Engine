// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Core/AXSystem.h"
#include "AXUpdateables.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXInterface.h"

class AXWindow : public AXParent< AXSystem< AXWindow >, AXWindow >
				, public AXImplementsInterface< AXIUpdateable >
{
public:
	/**
	* Constructor
	*/
	AXWindow();
	
	/**
	* Destructor
	*/
	~AXWindow();

	/**
	* Initialize the system, called after settings are loaded
	*/
	virtual InitResult OnInitialize( ) override;

	/**
	* Shutdown the system
	*/
	virtual void OnShutdown( ) override;

	/**
	* Runs all windows messages
	*/
	void ProcessMessages( );

	/**
	* Call to update all updateable objects
	*/
	virtual void Update( float dt ) override;

	/**
	 * Returns the window handle
	 */
	struct HWND__& GetWindowHandle( ) { return AXUtils::AssertPtrReturnRef( mHwnd ); }

private:
	/**
		* Application instance handle
		*/
	struct HINSTANCE__* mHInstance = nullptr;

	/**
		* The window handle
		*/
	struct HWND__* mHwnd = nullptr;
};
