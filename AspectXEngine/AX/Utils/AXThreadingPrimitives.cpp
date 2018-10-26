// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXThreadingPrimitives.h"

/**
* Attempts to get a write lock on the object, returning a success flag, will not block thread executionit is possible
	* for this function to exist in a case where obtaining a write lock is valid
*/
bool AXMultiReadLock::TryWriteLock( const void* object )
{
	bool expectedValue = false;
	if( mInternalStateLocked.compare_exchange_strong( expectedValue, true ) )
	{
		if( mReadLocks.empty( ) )
		{
			AXMULTIREADLOCKPTRINT expectedValue = 0;
			mWriteLock.compare_exchange_strong( expectedValue, ( AXMULTIREADLOCKPTRINT )object );
		}

		mInternalStateLocked = false;
	}

	return HasWriteLock( object );
}

/**
* Will block thread execution until a write lock can be obtained
*/
void AXMultiReadLock::WriteLock( const void* object )
{
	do {  } while ( !TryWriteLock( object ) );
}

/**
* Releases a write lock
*/
void AXMultiReadLock::ReleaseWriteLock( const void* object )
{
	AXMULTIREADLOCKPTRINT expectedValue = ( AXMULTIREADLOCKPTRINT )object;
	mWriteLock.compare_exchange_strong( expectedValue, 0 );
}

/**
* Returns true if the passed in object currently has a write lock
*/
bool AXMultiReadLock::HasWriteLock( const void* object )
{
	return ( mWriteLock == ( AXMULTIREADLOCKPTRINT )object );
}

/**
* Attempts to get a Read lock on the object, returning a success flag, will not block thread execution, it is possible
	* for this function to exist in a case where obtaining a read lock is valid
*/
bool AXMultiReadLock::TryReadLock( const void* object )
{
	bool expectedValue = false;
	if( mInternalStateLocked.compare_exchange_strong( expectedValue, true ) )
	{
		if( mWriteLock )
		{
			mInternalStateLocked = false;
			return false;
		}

		mReadLocks.push_back( ( AXMULTIREADLOCKPTRINT )object );
		mInternalStateLocked = false;
		return true;
	}

	return false;
}

/**
* Will block thread execution until a Read lock can be obtained
*/
void AXMultiReadLock::ReadLock( const void* object )
{
	do { } while ( !TryReadLock( object ) );
}

/**
* Releases a Read lock
*/
void AXMultiReadLock::ReleaseReadLock( const void* object )
{
	bool done( false );

	do
	{
		bool expectedValue = false;
		if( mInternalStateLocked.compare_exchange_strong( expectedValue, true ) )
		{
			mReadLocks.remove( ( AXMULTIREADLOCKPTRINT)object );

			mInternalStateLocked = false;
			done = true;
		}

	} while( !done );
}

/**
* Returns true if the passed in object currently has a Read lock
*/
bool AXMultiReadLock::HasReadLock( const void* object )
{
	if( mWriteLock )
	{
		return false;
	}

	do
	{
		bool expectedValue = false;
		if( mInternalStateLocked.compare_exchange_strong( expectedValue, true ) )
		{
			for( auto& item : mReadLocks )
			{
				if( item == ( AXMULTIREADLOCKPTRINT )object )
				{
					mInternalStateLocked = false;
					return true;
				}
			}
			
			mInternalStateLocked = false;
			return false;
		}

	} while( true );

	return false;
}

/**
* Less efficient way of unlocking, attempts to unlock both a read or a write lock
*/
void AXMultiReadLock::ReleaseLock( const void* object )
{
	ReleaseReadLock( object );
	ReleaseWriteLock( object );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Constructor
*/
AXMultiReadLock_ScopedRead::AXMultiReadLock_ScopedRead( AXMultiReadLock& lock )
	: mLock( lock )
{
	mLock.ReadLock( this );
}

/**
* Destructor
*/
AXMultiReadLock_ScopedRead::~AXMultiReadLock_ScopedRead( )
{
	mLock.ReleaseReadLock( this );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Constructor
*/
AXMultiReadLock_ScopedWrite::AXMultiReadLock_ScopedWrite( AXMultiReadLock& lock )
	: mLock( lock )
{
	mLock.WriteLock( this );
}

/**
* Destructor
*/
AXMultiReadLock_ScopedWrite::~AXMultiReadLock_ScopedWrite( )
{
	mLock.ReleaseWriteLock( this );
}
