// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXString.h"
#include "AX/Utils/AXBaseObject.h"
#include "AX/Utils/AXParent.h"

class AXFile : public AXParent< AXBaseObject, AXFile >
{
public:
#if defined( AX32BIT )
	using FileSize = uint32_t;
#elif defined( AX64BIT )
	using FileSize = uint64_t;
#endif

	enum class FileOpenMode
	{
		Read,
		Write,
		Append,
		ReadWrite,
		Truncate,
	};

	enum class DataMode
	{
		Normal,
		Binary,
	};

	struct InternalFileBuffer
	{
		uint8_t* mBuffer = nullptr;
		FileSize mBufferSize = 0;
	};

public:
	/**
	* Constructor
	*/
	AXFile();

	/**
	* Open file Constructor
	*/
	AXFile( const AXString& path, FileOpenMode fop, DataMode dm = AXFile::DataMode::Normal );

	/**
	* Destructor
	*/
	virtual ~AXFile();
	
	/**
	 * Opens a file, returns true if the file was successfully
	 * opened.
	 */
	virtual bool OpenFile( const AXString& path, FileOpenMode fop, DataMode dm = AXFile::DataMode::Normal );

	/**
	 * Returns if the file is open
	 */
	virtual bool IsOpen( ) const;

	/**
	 * Closes the file, leaving internal buffer intact
	 */
	virtual void CloseFile( );

	/**
	 * Returns the size of the file
	 */
	virtual FileSize GetFileSize( ) const;

	/**
	 * File must be open, reads the contents from the file into the internal buffer and returns the internal buffer
	 */
	virtual InternalFileBuffer& ReadFileToInternalBuffer( );

	/**
	 * File must be open, writes the contents of the internal buffer to the file
	 */
	virtual void WriteInternalBufferToFile( );

	/**
	 * Returns a reference to the internal buffer
	 */
	InternalFileBuffer& GetInternalBuffer( );

	/**
	* Returns a copy of the internal buffer then clear out our version
	*/
	InternalFileBuffer ObtainInternalBuffer( );

	/**
	 * Cleansup the internal buffer if it is already allocated, allocates a new internal buffer of size. if data != nullptr copies the data in
	 */
	InternalFileBuffer& CreateInternalBuffer( FileSize size, uint8_t* data = nullptr );

	/**
	 * Destroys our internal buffer
	 */
	void DestroyInternalBuffer( );

public:

	// Helpers

	/**
	 * Returns the extention from the given path
	 */
	static AXString GetExtention( const AXString& path );

protected:
	/**
	 * Handles our internal buffer
	 */
	InternalFileBuffer mInternalFileBuffer;
};