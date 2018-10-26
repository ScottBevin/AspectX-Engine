// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"
#include "AX/Core/AXLogging.h"

#include "AX/Utils/AXString.h"
#include <map>

class AXContentManagerBase : public AXParent< AXBaseObject, AXContentManagerBase >
{
public:
	AXContentManagerBase( const AXString& name ) : mName( name ) { }

	/**
	 * Returns true if this content manager can handle the given extention
	 */
	bool IsExtentionSupported( const AXString& ext ) { return ( mContentImporters.find( ext ) != mContentImporters.end( ) ); }

	/**
	 * Return the name of this manager
	 */
	const AXString& GetName( ) const { return mName; }

	/**
	* Override to register a settings object for this manager
	*/
	virtual void CreateSettings( class AXSettingsFile& settings ) { }

	/**
	* Override to return a pointer to a valid settings item if one exists
	*/
	virtual AXSettingsFile::SettingsItem* GetSettings( ) { return nullptr; }

	/**
	 * Returns a reference to the content importers
	 */
	const auto& GetImporters( ) const { return mContentImporters; }

protected:
	/**
	* Initialises a new content importer
	*/
	void InitialiseNewImporter( class AXContentImporterBase& importer );

protected:
	/**
	 * The name of this content manager
	 */
	AXString mName;

	/**
	 * Holds all the extentions this content manager can support
	 */
	std::map< AXString, class AXContentImporterBase* > mContentImporters;
};

template< class TAssetType >
class AXContentManager : public AXParent< AXContentManagerBase, AXContentManager< TAssetType > >
{
	static_assert( std::is_base_of< class AXAssetBase, TAssetType >::value, "TAssetType must derive from AXAsset" );

public:
	
	using AssetType = TAssetType;
	typedef typename TAssetType::Handle Handle;

	AXContentManager() : AXParent< AXContentManagerBase, AXContentManager< TAssetType > >( sContentManagerName ) { }

	template< class TImporter >
	void RegisterContentImporter( )
	{
		static_assert( std::is_base_of< class AXContentImporter< TAssetType >, TImporter >::value, "TImporter must be an importer that supports TAssetType" );

		if( IsExtentionSupported( TImporter::StaticGetSupportedExtention( ) ) )
		{
			AXERROR( "Content", "Attempting to register importer for an extention type that already has a registered importer." );
			return;
		}

		TImporter* newImporter( new TImporter( ) );
		InitialiseNewImporter( *newImporter );
		mContentImporters[TImporter::StaticGetSupportedExtention( )] = newImporter;
	}

	static const AXString& Name( ) { return sContentManagerName; }

private:
	static AXString sContentManagerName;
};