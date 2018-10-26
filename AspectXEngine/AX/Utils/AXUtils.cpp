// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXUtils.h"
#include "AX/Core/AXLogging.h"

/**
* Asserts condition, errors if the value is false
*/
void AXUtils::AssertFailed2( const char* path, int line, const AXString& message )
{
	AXLogging::Log_Internal( AXLogging::LogLevel::Error, "Assert", path, line, message.c_str( ) );
	assert( false ); // Because crash on error may be off
}

/**
* Asserts condition, throws a warning if the value is false
*/
void AXUtils::AssertWarning2( const char* path, int line, const AXString& message )
{
	AXLogging::Log_Internal( AXLogging::LogLevel::Warning, "Assert", path, line, message.c_str( ) );
}

/**
* Splits str into segments seperater by seperator into outResults
*/
void AXUtils::SplitString( const AXString& str, char seperator, std::vector< AXString >& outResults )
{
	if( str.empty( ) )
	{
		return;
	}

	const char* s = str.c_str( );
	AXString::size_type strLen( str.length( ) );
	uint32_t subStrStart( 0 );
	uint32_t subStrCount( 0 );

	for( AXString::size_type i( 0 ); i < strLen; ++i, ++s, ++subStrCount )
	{
		if( (*s) == seperator )
		{
			outResults.push_back( str.substr( subStrStart, subStrCount ) );

			subStrStart += subStrCount + 1;
			subStrCount = 0;

			++s;
			++i;
		}
	}

	if( subStrCount > 0 )
	{
		outResults.push_back( str.substr( subStrStart, subStrCount ) );
	}
}