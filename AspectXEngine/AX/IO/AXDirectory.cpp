// Copyright 2016 Scott Bevin, All Rights Reserved

#include <iostream>
#include <corecrt_io.h>

#include "AXDirectory.h"
#include "AX/Core/AXLogging.h"
#include <wtypes.h>

/**
* Clears out the current directory results and reads the results in from the new path, returns true if the directory exists
*/
bool AXDirectory::Open( const AXString& path )
{
	mContents.clear( );
	mPath = path;

	if( Exists( path ) )
	{
		AXString searchPath = path + "/*";

		struct _finddata_t findData;
		intptr_t fHndl;

		if( ( fHndl = _findfirst( searchPath.c_str(), &findData ) ) > 0 )
		{
			do
			{
				if( findData.name[0] != '.' )
				{
					Item item;
					item.mName = findData.name;
					item.mType = ( ( ( findData.attrib & _A_SUBDIR ) != 0 ) ? Item::Type::Directory : Item::Type::File );

					mContents.push_back( item );
				}

			} while( _findnext( fHndl, &findData ) == 0 );

			return true;
		}
	}

	return false;
}

/**
* Sets up a watch on this directory, auto-refreshes if the directory changes
*/
void AXDirectory::WatchForChanges( )
{
	AXLOG_UNIMPLEMENTED_FUNCTION
}

/**
* Returns true if this directory exists
*/
bool AXDirectory::Exists( const AXString& path )
{
	DWORD dwAttrib = GetFileAttributes( path.c_str() );

	return ( dwAttrib != INVALID_FILE_ATTRIBUTES &&
		( dwAttrib & FILE_ATTRIBUTE_DIRECTORY ) );
}