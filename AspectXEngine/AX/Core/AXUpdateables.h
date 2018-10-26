// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include <list>

#include "AX/Core/AXSystem.h"
#include "AX/Utils/AXInterface.h"

/**
 * An interface that can be derived to make an object update
 */
class AXIUpdateable : public AXInterface< AXIUpdateable >
{
public:
	/**
	* Constructor
	*/
	AXIUpdateable( );

	/**
	* Destructor
	*/
	~AXIUpdateable( );

	/**
	 * Call to update all updateable objects
	 */
	virtual void Update( float dt ) { }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXUpdateables : public AXParent< AXSystem< AXUpdateables >, AXUpdateables >
{
public:
	/**
	 * Update all the updateable objects
	 */
	virtual void Update( float dt ) override;
	
	friend class AXIUpdateable;

private:
	/**
	 * All the update able objects
	 */
	std::list< AXIUpdateable* > mUpdateableObjects;
};