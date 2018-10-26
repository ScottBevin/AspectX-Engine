// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXUtils.h"
#include "AX/Core/AXSystem.h"
#include "AX/Core/AXSettings.h"
#include "AX/Utils/AXBuffer.h"

class AXFileSystem : public AXParent< AXSystem< AXFileSystem >, AXFileSystem >
{
public:
	class Settings : public AXParent< AXSettingsFile::SettingsItem, Settings >
	{
	public:

		/**
		* Constructor
		*/
		Settings( )
		{
		}

	public:
	};


public:

	Settings& GetSettings( ) { return AXUtils::AssertPtrReturnRef( mSettings ); }

	/**
	* Initialise the system, called after settings are loaded
	*/
	virtual InitResult OnInitialize( ) override;

	/**
	* Called once a frame to allow systems to update
	*/
	virtual void Update( float dt ) override;

	/**
	* Shutdown the system
	*/
	virtual void OnShutdown( ) override;

	struct FileTransactionParams
	{
		struct Flags
		{
			enum E
			{
				/**
				 * The transaction will block until performed
				 */
				Blocking = ( 0x1 << 0 ),
			};

			using Mask = uint8_t;
		};

		/**
		 * The filepath we are reading
		 */
		AXString mFilePath;

		/**
		 * An offset into the file to read
		 */
		uint64_t mOffset;

		/**
		 * The amount to read from the file, if 0 will use the size in mBuffer, if that is 0, the read will fail
		 */
		uint64_t mSize;

		/**
		 * An allocated buffer to read the file into, the size of the buffer indicates the amount that will be requested read 
		 * from the file is mReadAmount is 0
		 */
		AXBuffer64 mBuffer;

		/**
		 * Flags passed in to identify how the system should process the file request
		 */
		Flags::Mask mFlags = 0;
	};

	/**
	 * Creates a new file and queues it for an async read, returns a pointer to the file that
	 * must be destroyed by calling DestroyFile() when finished.
	 */
	class AXFile* ReadFile( const FileTransactionParams& params );

	/**
	 * Cleanly destroys the file and any associated memory in a thread-safe manner
	 */
	void DestroyFile( class AXFile& file );

protected:
	/**
	* Override to register a settings object for this system
	*/
	virtual void CreateEngineSettings( class AXSettingsFile& settings ) override;

private:
	/**
	* Pointer to the created settings object
	*/
	Settings* mSettings = nullptr;

};