// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Core/AXApplication.h"

class TestProjectApplication : public AXApplication
{
public:
	/**
	* Constructor
	*/
	TestProjectApplication();

	/**
	* Destructor
	*/
	~TestProjectApplication();
	
protected:
	/**
	* Creates game systems
	*/
	virtual void CreateDefaultSystems( ) override;

private:

};