// Copyright 2016 Scott Bevin, All Rights Reserved

#if defined( AXPLATFORM_WINDOWS )

#include "AXFile_Windows.h"
#include "AX\Utils\AXUtils.h"
#include <xiosbase>

/**
* Opens a file, returns true if the file was successfully
* opened.
*/
bool AXFile_Windows::OpenFile( const AXString& path, FileOpenMode fop, DataMode dm )
{
	if( IsOpen( ) )
	{
		CloseFile( );
	}

	std::ios_base::open_mode openMode = 0x0;

	switch( fop )
	{
		case FileOpenMode::Write: { openMode |= std::ios::out; } break;
		case FileOpenMode::Read: { openMode |= std::ios::in; } break;
		case FileOpenMode::Append: { openMode |= std::ios::app; } break;
		case FileOpenMode::Truncate: { openMode |= std::ios::trunc; } break;
		case FileOpenMode::ReadWrite: { openMode |= std::ios::out | std::ios::in; } break;
	};

	if( dm == DataMode::Binary )
	{
		openMode |= std::ios::binary;
	}

	mFile.open( path, openMode );

	return IsOpen( );
}

/**
* Returns if the file is open
*/
bool AXFile_Windows::IsOpen( ) const
{
	return mFile.is_open( );
}

/**
* Closes the file, leaving internal buffer intact
*/
void AXFile_Windows::CloseFile( )
{
	mFile.close( );
}

/**
* Returns the file stream
*/
std::fstream& AXFile_Windows::GetFile( )
{
	return mFile;
}

/**
* Returns the size of the file
*/
AXFile_Windows::FileSize AXFile_Windows::GetFileSize( ) const
{
	if( !IsOpen( ) )
	{
		return 0;
	}

	std::fstream& file( const_cast< std::fstream& >( mFile ) );

	std::streampos currentPos( file.tellg( ) );
	file.seekg( 0, std::ios::end );
	std::streampos endPos( file.tellg( ) );
	file.seekg( 0, std::ios::beg );
	std::streampos startPos( file.tellg( ) );
	file.seekg( currentPos );

	return ( FileSize )( endPos - startPos );
}

/**
* File must be open, reads the contents from the file into the internal buffer and returns the internal buffer
*/
AXFile_Windows::InternalFileBuffer& AXFile_Windows::ReadFileToInternalBuffer( )
{
	FileSize filesize( GetFileSize( ) );
	CreateInternalBuffer( filesize );

	if( filesize > 0 )
	{
		AXASSERT0( mInternalFileBuffer.mBufferSize == filesize );
		mFile.read( ( char* )mInternalFileBuffer.mBuffer, mInternalFileBuffer.mBufferSize );
	}

	return mInternalFileBuffer;
}

/**
* File must be open, writes the contents of the internal buffer to the file
*/
void AXFile_Windows::WriteInternalBufferToFile( )
{
	if( IsOpen( ) && mInternalFileBuffer.mBuffer && mInternalFileBuffer.mBufferSize > 0 )
	{
		mFile.write( ( char* )mInternalFileBuffer.mBuffer, mInternalFileBuffer.mBufferSize );
	}
}

#endif // #if defined( AXPLATFORM_WINDOWS )