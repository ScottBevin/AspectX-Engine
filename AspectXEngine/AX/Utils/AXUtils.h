// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXString.h"
#include <memory>
#include <vector>

#include "Libs/cJSON/cJSON.h"

#define AXASSERT( COND, MSG, ... ) do{ if( !(COND) ) { AXUtils::AssertFailed( __FILE__, __LINE__, MSG, __VA_ARGS__ ); }  } while( false )
#define AXASSERT0( COND ) do{ if( !(COND) ) { AXUtils::AssertFailed( __FILE__, __LINE__, "Assert" ); }  } while( false )

#define AXASSERT_WARN( COND, MSG, ... ) do{ if( !(COND) ) { AXUtils::AssertWarning( __FILE__, __LINE__, MSG, __VA_ARGS__ ); }  } while( false )
#define AXASSERT0_WARN( COND ) do{ if( !(COND) ) { AXUtils::AssertWarning( __FILE__, __LINE__, "Assert" ); }  } while( false )

#define AXJOIN2( X, Y ) X ## Y
#define AXJOIN( X, Y ) AXJOIN2( X, Y )

class AXUtils
{
public:
	template< class T >
	static void ZeroStruct( T& obj )
	{
		memset( &obj, 0, sizeof( obj ) );
	}

	template< class T >
	static T& AssertPtrReturnRef( T* ptr )
	{
		AXASSERT( ptr, "Ptr assertion failed" );
		return ( *ptr );
	}

	template< class T >
	static const T& AssertPtrReturnRef( const T* ptr )
	{
		AXASSERT( ptr, "Ptr assertion failed" );
		return ( *ptr );
	}

	template< typename ... Args >
	static AXString FormatString( const AXString& format, Args ... args )
	{
		size_t size = snprintf( nullptr, 0, format.c_str( ), args ... ) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf( new char[size] );
		snprintf( buf.get( ), size, format.c_str( ), args ... );
		return AXString( buf.get( ), buf.get( ) + size - 1 ); // We don't want the '\0' inside
	}

	/**
	* Asserts condition, errors if the value is false
	*/
	static void AssertFailed2( const char* path, int line, const AXString& message );

	/**
	 * Asserts condition, errors if the value is false
	 */
	template< typename ... Args >
	static void AssertFailed( const char* path, int line, const AXString& message, Args ... args )
	{
		AssertFailed2( path, line, FormatString( message, args ... ) );
	}

	/**
	* Asserts condition, throws a warning if the value is false
	*/
	static void AssertWarning2( const char* path, int line, const AXString& message );

	/**
	* Asserts condition, throws a warning if the value is false
	*/
	template< typename ... Args >
	static void AssertWarning( const char* path, int line, const AXString& message, Args ... args )
	{
		AssertWarning2( path, line, FormatString( message, args ... ) );
	}

	/**
	 * Splits str into segments seperater by seperator into outResults
	 */
	static void SplitString( const AXString& str, char seperator, std::vector< AXString >& outResults );

	template< class T1, class T2 >
	static const auto& Min( const T1& a, const T2& b )
	{
		return ( ( a < b ) ? a : b );
	}

	template< class T1, class T2 >
	static auto& Min( T1& a, T2& b )
	{
		return ( ( a < b ) ? a : b );
	}

	template< class T1, class T2 >
	static const auto& Max( const T1& a, const T2& b )
	{
		return ( ( a > b ) ? a : b );
	}

	template< class T1, class T2 >
	static auto& Max( T1& a, T2& b )
	{
		return ( ( a > b ) ? a : b );
	}
};