// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXFile.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Core/AXLogging.h"

/**
* Constructor
*/
AXFile::AXFile()
{

}

/**
* Open file Constructor
*/
AXFile::AXFile( const AXString& path, FileOpenMode fop, DataMode dm )
{
	OpenFile( path, fop, dm );
}

/**
* Destructor
*/
AXFile::~AXFile()
{
	DestroyInternalBuffer( );
	CloseFile( );
}

/**
* Opens a file, returns true if the file was successfully
* opened.
*/
bool AXFile::OpenFile( const AXString& path, FileOpenMode fop, DataMode dm )
{
	AXLOG_UNIMPLEMENTED_FUNCTION;
	return false;
}

/**
* Returns if the file is open
*/
bool AXFile::IsOpen( ) const
{
	AXLOG_UNIMPLEMENTED_FUNCTION;
	return false;
}

/**
* Closes the file, leaving internal buffer intact
*/
void AXFile::CloseFile( )
{
	AXLOG_UNIMPLEMENTED_FUNCTION;
}

/**
* Returns the size of the file
*/
AXFile::FileSize AXFile::GetFileSize( ) const
{
	AXLOG_UNIMPLEMENTED_FUNCTION;
	return 0;
}

/**
* File must be open, reads the contents from the file into the internal buffer and returns the internal buffer
*/
AXFile::InternalFileBuffer& AXFile::ReadFileToInternalBuffer( )
{
	AXLOG_UNIMPLEMENTED_FUNCTION;
	return mInternalFileBuffer;
}

/**
* File must be open, writes the contents of the internal buffer to the file
*/
void AXFile::WriteInternalBufferToFile( )
{
	AXLOG_UNIMPLEMENTED_FUNCTION;
}

/**
* Returns a reference to the internal buffer
*/
AXFile::InternalFileBuffer& AXFile::GetInternalBuffer( )
{
	return mInternalFileBuffer;
}

/**
* Returns a copy of the internal buffer then clear out our version
*/
AXFile::InternalFileBuffer AXFile::ObtainInternalBuffer( )
{
	AXFile::InternalFileBuffer fbCopy( mInternalFileBuffer );

	mInternalFileBuffer.mBuffer = nullptr;
	mInternalFileBuffer.mBufferSize = 0;

	return fbCopy;
}

/**
* Cleansup the internal buffer if it is already allocated, allocates a new internal buffer of size. if data != nullptr copies the data in
*/
AXFile::InternalFileBuffer& AXFile::CreateInternalBuffer( FileSize size, uint8_t* data )
{
	DestroyInternalBuffer( );

	if( size > 0 )
	{
		mInternalFileBuffer.mBufferSize = size;
		mInternalFileBuffer.mBuffer = new uint8_t[size];

		if( data )
		{
			memcpy( mInternalFileBuffer.mBuffer, data, sizeof( uint8_t ) * size );
		}
	}

	return mInternalFileBuffer;
}

/**
* Destroys our internal buffer
*/
void AXFile::DestroyInternalBuffer( )
{
	delete[] mInternalFileBuffer.mBuffer;

	mInternalFileBuffer.mBuffer = nullptr;
	mInternalFileBuffer.mBufferSize = 0;
}

/**
* Returns the extention from the given path
*/
AXString AXFile::GetExtention( const AXString& path )
{
	AXString::size_type periodLoc( path.find_first_of( '.' ) );
	AXString::size_type strLen( path.length( ) );

	if( periodLoc == strLen )
	{
		return "";
	}

	return path.substr( periodLoc, strLen - periodLoc );
}