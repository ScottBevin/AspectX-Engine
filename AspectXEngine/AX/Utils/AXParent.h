// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AXBaseObject.h"
#include <type_traits>

template< class TBase, class TDerived >
class AXParent : public TBase
{
	static_assert( std::is_base_of< AXBaseObject, TBase >::value, "Classes using AXParent must derive from AXObject" );

public:
	using Parent = TBase;

	template< typename ... Args >
	AXParent( Args... args ) : TBase( args ... ) { }

	/**
	* Destructor
	*/
	virtual ~AXParent( ) { }
};
