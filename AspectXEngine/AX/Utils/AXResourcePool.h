// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AXHandle.h"
#include "AXParent.h"
#include "AXBaseObject.h"
#include "AXUtils.h"
#include "AXThreadingPrimitives.h"
#include "AX/Utils/AXVector.h"
#include "AX/Core/AXLogging.h"

using AXResourcePoolHandle = AXHandle;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * An allocator that supports a fixed size resource pool, size specified when the pool is created
 */
template< class TResourceType, class THandleType >
class AXResourcePool_StorageType_FixedSize
{
public:
	using ResourceType = TResourceType;
	using Handle = THandleType;
	typedef typename Handle::IdType SizeType;

private:
	struct ResourceItemMeta
	{
		THandleType mCurrentlyValidHandle = THandleType::Invalid;
		std::atomic< bool > mInUse = false;
	};

public:
	AXResourcePool_StorageType_FixedSize( const SizeType& size )
		: mItems( size )
		, mMetas( size )
	{
		AXASSERT( size < THandleType::MaxId, "Handle type does not provide support for %d number of items", size );
	}

	/**
	* Allocates an object within the pool if possible, returns a handle to the allocated object if one was allocated.
	* If allocatedObject is not nullptr will fill it in pointing to the object allocated
	*/
	Handle Allocate( ResourceType** allocatedObject = nullptr )
	{ 
		for( SizeType i( 0 ); i < mItems.size(); ++i )
		{
			ResourceType& item( mItems[i] );
			ResourceItemMeta& meta( mMetas[i] );

			bool expectedInUseFlag = false;
			if( meta.mInUse.compare_exchange_strong( expectedInUseFlag, true ) )
			{
				meta.mCurrentlyValidHandle = Handle::Create( i );

				if( allocatedObject )
				{
					( *allocatedObject ) = &item;
				}

				AXASSERT( mNumInUse < Capacity( ), "Something has gone wrong inside a resource pool..." );
				++mNumInUse;

				return meta.mCurrentlyValidHandle;
			}
		}

		AXWARN( "Resource Pool", "Unable to allocate inside fixed size resource pool" );

		return Handle::Invalid;
	}

	/**
	* Released the object pointed to by the handle, if the handle is valid
	*/
	void Release( Handle& hndl )
	{ 
		ResourceType& item( mItems[hndl.Id( )] );
		ResourceItemMeta& meta( mMetas[hndl.Id( )] );

		bool expectedInUseFlag = true;
		if( meta.mCurrentlyValidHandle == hndl && meta.mInUse.compare_exchange_strong( expectedInUseFlag, false ) )
		{
			meta.mCurrentlyValidHandle = Handle::Invalid;
			item.~ResourceType( );

			AXASSERT( mNumInUse > 0, "Something has gone wrong inside a resource pool..." );
			--mNumInUse;
		}

		hndl = Handle::Invalid;
	}

	/**
	* Attempts to get a resource from the given handle, returns it if possible, otherwise returns nullptr
	*/
	const ResourceType* TryGet( const Handle& hndl ) const
	{ 
		ResourceType& item( mItems[hndl.Id( )] );
		ResourceItemMeta& meta( mMetas[hndl.Id( )] );

		if( meta.mInUse && meta.mCurrentlyValidHandle == hndl )
		{
			return &item;
		}

		return nullptr;
	}

	/**
	* Attempts to get a resource from the given handle, returns it if possible, otherwise returns nullptr
	*/
	ResourceType* TryGet( const Handle& hndl )
	{ 
		ResourceType& item( mItems[hndl.Id( )] );
		ResourceItemMeta& meta( mMetas[hndl.Id( )] );

		if( meta.mInUse && meta.mCurrentlyValidHandle == hndl )
		{
			return &item;
		}

		return nullptr;
	}

	/**
	* Returns the maximum capacity of this resource pool
	*/
	SizeType Capacity( ) const 
	{ 
		return static_cast< SizeType >( mItems.size( ) );
	}

	/**
	* Returns the currently used capacity of this resource pool
	*/
	SizeType Count( ) const 
	{ 
		return mNumInUse;
	}

	/**
	* Returns a reference to the begin element of the internal storage items so they can all be iterated over
	*/
	auto begin( ) { return mItems.begin( ); }

	/**
	* Returns a reference to the begin element of the internal storage of items so they can all be iterated over
	*/
	auto end( ) { return mItems.end( ); }

private:
	AXVector< TResourceType > mItems;
	AXVector< ResourceItemMeta > mMetas; 
	AXAtomic< typename SizeType > mNumInUse = 0;
};

/**
 * An allocator that supports a fixed size resource pool, size specified at compile time
 */
template< class TResourceType, class THandleType, uint64_t TSize >
class AXResourcePool_StorageType_StaticFixedSize : public AXResourcePool_StorageType_FixedSize< TResourceType, THandleType >
{
public:
	static_assert( TSize < THandleType::MaxId, "Handle type does not provide support for TSize number of items" );
	static const SizeType Size = TSize;

	AXResourcePool_StorageType_StaticFixedSize() : AXResourcePool_StorageType_FixedSize( Size ) { }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class TResourceType, class THandleType, class TResourcePoolStorageType >
class AXResourcePool : public AXParent< AXBaseObject, AXResourcePool< TResourceType, THandleType, TResourcePoolStorageType > >
{
public:
	using ResourceType = TResourceType;
	using ResourcePoolStorageType = TResourcePoolStorageType;
	using Handle = THandleType;

	typedef typename TResourcePoolStorageType::SizeType SizeType;

public:
	template< typename ... Args >
	AXResourcePool( Args... args ) : mStorage( args ... ) { }

	/**
	 * Allocates an object within the pool if possible, returns a handle to the allocated object if one was allocated.
	 * If allocatedObject is not nullptr will fill it in pointing to the object allocated
	 */
	Handle Allocate( ResourceType** allocatedObject = nullptr ) { return mStorage.Allocate( allocatedObject ); }

	/**
	 * Released the object pointed to by the handle, if the handle is valid
	 */
	void Release( Handle& hndl ) { mStorage.Release( hndl ); }

	/**
	 * Attempts to get a resource from the given handle, returns it if possible, otherwise returns nullptr
	 */
	const ResourceType* TryGet( const Handle& hndl ) const { return mStorage.TryGet( hndl ); }

	/**
	* Attempts to get a resource from the given handle, returns it if possible, otherwise returns nullptr
	*/
	ResourceType* TryGet( const Handle& hndl ) { return mStorage.TryGet( hndl ); }

	/**
	* Attempts to get a resource from the given handle, returns it if possible, otherwise returns nullptr
	*/
	const ResourceType& Get( const Handle& hndl ) const { return AXUtils::AssertPtrReturnRef( Get( hndl ) ); }

	/**
	* Attempts to get a resource from the given handle, returns it if possible, otherwise returns nullptr
	*/
	ResourceType& Get( const Handle& hndl ) { return AXUtils::AssertPtrReturnRef( Get( hndl ) ); }

	/**
	 * Returns a reference to the begin element of the internal storage items so they can all be iterated over
	 */
	auto begin( ) { return mStorage.begin( ); }

	/**
	* Returns a reference to the begin element of the internal storage of items so they can all be iterated over
	*/
	auto end( ) { return mStorage.end( ); }

	/**
	 * Returns the maximum capacity of this resource pool
	 */
	SizeType Capacity( ) const { return mStorage.Capacity( ); }

	/**
	* Returns the currently used capacity of this resource pool
	*/
	SizeType Count( ) const { return mStorage.Count( ); }

private:
	ResourcePoolStorageType mStorage;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class TResourceType, uint64_t TSize, class THandleType = AXResourcePoolHandle >
using AXStaticFixedSizeResourcePool = AXResourcePool< TResourceType, THandleType, AXResourcePool_StorageType_StaticFixedSize< TResourceType, THandleType, TSize > >;

template< class TResourceType, class THandleType = AXResourcePoolHandle >
using AXFixedSizeResourcePool = AXResourcePool< TResourceType, THandleType, AXResourcePool_StorageType_FixedSize< TResourceType, THandleType > >;