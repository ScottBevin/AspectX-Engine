// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#if defined( AXPLATFORM_WINDOWS )

#include <fstream>
#include "AX/IO/AXFile.h"

class AXFile_Windows : public AXParent< AXFile, AXFile_Windows >
{
public:
	using AXParent::AXParent;

	/**
	* Opens a file, returns true if the file was successfully
	* opened.
	*/
	virtual bool OpenFile( const AXString& path, FileOpenMode fop, DataMode dm = AXFile::DataMode::Normal ) override;

	/**
	* Returns if the file is open
	*/
	virtual bool IsOpen( ) const override;

	/**
	* Closes the file, leaving internal buffer intact
	*/
	virtual void CloseFile( ) override;

	/**
	* Returns the file stream
	*/
	std::fstream& GetFile( );

	/**
	* Returns the size of the file
	*/
	virtual FileSize GetFileSize( ) const override;

	/**
	* File must be open, reads the contents from the file into the internal buffer and returns the internal buffer
	*/
	virtual InternalFileBuffer& ReadFileToInternalBuffer( ) override;

	/**
	* File must be open, writes the contents of the internal buffer to the file
	*/
	virtual void WriteInternalBufferToFile( ) override;

private:
	/**
	* The file we have open
	*/
	std::fstream mFile;
};

#endif // #if defined( AXPLATFORM_WINDOWS )