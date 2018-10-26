// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AXParent.h"
#include "AXBaseObject.h"

#include <thread>
#include <atomic>
#include <list>

#if defined( AX32BIT )
#define AXMULTIREADLOCKPTRINT uint32_t
#elif defined( AX64BIT )
#define AXMULTIREADLOCKPTRINT uint64_t
#endif 

template< typename T >
using AXAtomic = std::atomic< T >;

using AXThread = std::thread;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXMultiReadLock : public AXParent< AXBaseObject, AXMultiReadLock >
{
public:


public:
	/**
	* Attempts to get a write lock on the object, returning a success flag, will not block thread execution, it is possible
	* for this function to exist in a case where obtaining a write lock is valid
	*/
	bool TryWriteLock( const void* object );

	/**
	* Will block thread execution until a write lock can be obtained
	*/
	void WriteLock( const void* object );

	/**
	* Releases a write lock
	*/
	void ReleaseWriteLock( const void* object );

	/**
	* Returns true if the passed in object currently has a write lock
	*/
	bool HasWriteLock( const void* object );

	/**
	* Attempts to get a Read lock on the object, returning a success flag, will not block thread execution, it is possible
	* for this function to exist in a case where obtaining a read lock is valid
	*/
	bool TryReadLock( const void* object );

	/**
	* Will block thread execution until a Read lock can be obtained
	*/
	void ReadLock( const void* object );

	/**
	* Releases a Read lock
	*/
	void ReleaseReadLock( const void* object );

	/**
	* Returns true if the passed in object currently has a Read lock
	*/
	bool HasReadLock( const void* object );

	/**
	 * Less efficient way of unlocking, attempts to unlock both a read or a write lock
	 */
	void ReleaseLock( const void* object );

private:

	/**
	 * Used as a memory barrier to lock the internal state of the object
	 */
	volatile AXAtomic< bool > mInternalStateLocked = false;

	/**
	* != nullptr when something has a write lock on this object
	*/
	volatile AXAtomic< AXMULTIREADLOCKPTRINT > mWriteLock = 0;

	/**
	* Lists all the objects with read locks
	*/
	std::list< AXMULTIREADLOCKPTRINT > mReadLocks;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXMultiReadLock_ScopedRead
{
public:
	/**
	 * Constructor
	 */
	AXMultiReadLock_ScopedRead( AXMultiReadLock& lock );

	/**
	 * Destructor
	 */
	~AXMultiReadLock_ScopedRead( );

private:
	/**
	 * The lock that we are actually working with
	 */
	AXMultiReadLock& mLock;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXMultiReadLock_ScopedWrite
{
public:
	/**
	* Constructor
	*/
	AXMultiReadLock_ScopedWrite( AXMultiReadLock& lock );

	/**
	* Destructor
	*/
	~AXMultiReadLock_ScopedWrite( );

private:
	/**
	* The lock that we are actually working with
	*/
	AXMultiReadLock& mLock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class T >
class AXMultiReadLockedObject : public AXParent< AXMultiReadLock, AXMultiReadLockedObject< T > >
{
public:

	template< typename ... Args >
	AXMultiReadLockedObject( Args... args ) : mObj( args ... ) { }

	/**
	 * Read locks the pointer then returns it, must call ReleaseLock when done with the pointer
	 */
	const T& GetRead( const void* object ) const
	{ 
		const_cast< AXMultiReadLockedObject* >( this )->ReadLock( object );
		return mObj; 
	}

	/**
	* Attempts to read lock the pointer and return it, must call ReleaseLock when done with the pointer
	*/
	const T* TryGetRead( const void* object ) const
	{
		if( const_cast< AXMultiReadLockedObject* >( this )->TryReadLock( object ) )
		{
			return &mObj;
		}

		return nullptr;
	}

	/**
	* Write locks the pointer then returns it, must call ReleaseLock when done with the pointer
	*/
	T& GetWrite( const void* object )
	{
		WriteLock( object );
		return mObj;
	}

	/**
	* Attempts to write lock the pointer and return it, must call ReleaseLock when done with the pointer
	*/
	T* TryGetWrite( const void* object )
	{
		if( TryWriteLock( object ) )
		{
			return &mObj;
		}

		return nullptr;
	}


private:
	/**
	 * The raw internal pointer
	 */
	T mObj = nullptr;
};