// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AXParent.h"
#include "AXBaseObject.h"
#include <stdint.h>

template< class TIdType, class TUniqueType >
class AXHandleBase : public AXParent< AXBaseObject, AXHandleBase< TIdType, TUniqueType > >
{
public:
	using IdType = TIdType;
	using UniqueType = TUniqueType;
	using HandleType = AXHandleBase< TIdType, TUniqueType >;

	AXHandleBase( const HandleType& hndl ) : mId( hndl.mId ), mUnique( hndl.mUnique ) { }

	const IdType& Id( ) const { return mId; }

	bool IsValid( ) const { return *this != Invalid; }

	// Operators

	operator bool( ) const { return IsValid( ); }

	operator const IdType&( ) const { return mId; }
	operator IdType( ) const { return mId; }

	HandleType& operator = ( const HandleType& hndl ) { mId = hndl.mId; mUnique = hndl.mUnique; return *this; }

	bool operator == ( const HandleType& hndl ) const { return ( mId == hndl.mId && mUnique == hndl.mUnique ); }

	bool operator != ( const HandleType& hndl ) const { return !( *this == hndl ); }

	static HandleType Create( )
	{
		static AXAtomic< IdType > nextId = 0;
		return Create( nextId++ );
	}

	static HandleType Create( const IdType &id )
	{
		return HandleType( id );
	}

protected:
	AXHandleBase( ) : AXHandleBase( Invalid ) { }
	AXHandleBase( const IdType &id ) : mId( id ) { AllocateNextUnique( mUnique ); }

	void AllocateNextUnique( UniqueType& unique )
	{
		static AXAtomic< UniqueType > nextUnique = 0;
		unique = nextUnique++;
	}

private:
	IdType mId;
	UniqueType mUnique;

public:
	static const HandleType Invalid;

	static const IdType MaxId = std::numeric_limits< TIdType >::max( );
};

template< class TIdType, class TUniqueType >
const AXHandleBase< TIdType, TUniqueType > AXHandleBase< TIdType, TUniqueType >::Invalid = AXHandleBase< TIdType, TUniqueType >( 0 );

using AXHandle = AXHandleBase< uint32_t, uint32_t >;