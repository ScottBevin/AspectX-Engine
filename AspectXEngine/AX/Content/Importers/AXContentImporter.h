// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"
#include "AX/Core/AXLogging.h"
#include "AX/IO/AXFile.h"

#include "AX/Utils/AXString.h"

class AXContentImporterBase : public AXParent< AXBaseObject, AXContentImporterBase >
{
public:
	AXContentImporterBase( const AXString& supportedExtention ) : mSupportedExtention( supportedExtention ) { }

	const AXString& GetSupportedExtention( ) const { return mSupportedExtention; }

	/**
	 * Call to import the asset specified by path, returns a valid pointer on success or nullptr on fail
	 */
	virtual class AXAssetBase* Import( const AXString& assetPath ) = 0;

	/**
	* Override to register a settings object for this importer
	*/
	virtual void CreateSettings( class AXSettingsFile& settings ) { }

	/**
	 * Override to return a pointer to a valid settings item if one exists
	 */
	virtual AXSettingsFile::SettingsItem* GetSettings( ) { return nullptr; }

private:
	AXString mSupportedExtention;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class TOutputType >
class AXContentImporter : public AXParent< AXContentImporterBase, AXContentImporter< TOutputType > >
{
	static_assert( std::is_base_of< class AXAssetBase, TOutputType >::value, "TOutputType must be an AXAsset" );

public:
	using OutputType = TOutputType;

public:
	AXContentImporter( ) : AXParent( sSupportedExtention ) {  }

	/**
	* Call to import the asset specified by path, returns a valid pointer on success or nullptr on fail
	*/
	virtual class AXAssetBase* Import( const AXString& assetPath ) final
	{
		// Validate the asset extension
		AXString ext( AXFile::GetExtention( assetPath ) );

		if( ext != sSupportedExtention )
		{
			AXWARN( "Content", "Failed import of file %s. Invalid extention.", ext.c_str() );
			return nullptr;
		}

		// Todo

		AXFile file;
		file.OpenFile( assetPath, AXFile::FileOpenMode::Read, AXFile::DataMode::Binary );

		if( !file.IsOpen( ) )
		{
			AXWARN( "Content", "Failed import of file %s. Failed to open file.", assetPath.c_str( ) );
			return nullptr;
		}

		AXFile::InternalFileBuffer& fileBuffer( file.ReadFileToInternalBuffer( ) );

		if( fileBuffer.mBufferSize <= 0 || !fileBuffer.mBuffer )
		{
			AXWARN( "Content", "Failed import of file %s. Failed to read file contents.", assetPath.c_str( ) );
			return nullptr;
		}

		class AXAssetBase* ret( ImportAsset( fileBuffer ) );

		if( !ret )
		{
			AXWARN( "Content", "Failed import of file %s. Failed import.", assetPath.c_str( ) );
			return nullptr;
		}

		return ret;
	}

	static const AXString& StaticGetSupportedExtention( ) { return sSupportedExtention; }

protected:
	/**
	 * Derived types should implement this function
	 */
	virtual OutputType* ImportAsset( const AXFile::InternalFileBuffer& fileBuffer ) = 0;

protected:
	static AXString sSupportedExtention;
};