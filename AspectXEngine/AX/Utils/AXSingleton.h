// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AXUtils.h"
#include "AXBaseObject.h"
#include "AXParent.h"

template< class T >
class AXSingleton : public AXParent< AXBaseObject, AXSingleton< T > >
{
public:

	template< class DerivedType >
	static T& Use( )
	{
		AXASSERT0( !sInstance );
		static_assert( std::is_base_of< T, DerivedType >::value, "DerivedType is incorrect." );

		sInstance = new DerivedType( );
		return *sInstance;
	}

	static T& Get( )
	{
		if( !sInstance )
		{
			sInstance = new T( );
		}

		return *sInstance;
	}

private:
	static T* sInstance;
};

template< class T >
T* AXSingleton< T >::sInstance = nullptr;