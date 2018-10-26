// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXString.h"
#include <map>
#include <type_traits>
#include <vector>

#include "AX/Utils/AXBaseObject.h"
#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXInterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXSystemBase : public AXBaseObject
{
public:
	enum class InitResult
	{
		Initialized,
		Retry,
		Failed,
	};

	enum class State
	{
		Uninitialized,
		Initialized,
		FailedToInitialize,
		Shutdown,
	};

public:
	/**
	* Constructor
	*/
	AXSystemBase( const AXString& name );

	/**
	* Destructor
	*/
	~AXSystemBase();
	
	const AXString& GetName( ) const { return mName; }

	State GetState( ) const { return mState; }

	/**
	 * Override to register a settings object for this system
	 */
	virtual void CreateEngineSettings( class AXSettingsFile& settings ){ }

	/**
	 * Override to handle command line arguments, gets called after create settings
	 */
	virtual void HandleCommandLine( const std::vector< AXString >& args ) { }

	/**
	* Initialise the system, called after settings are loaded
	*/
	InitResult Initialize( );
	
	/**
	* Shutdown the system
	*/
	void Shutdown( );

	/**
	 * Called at the start of a new frame
	 */
	virtual void BeginFrame( ) { }

	/**
	 * Called once a frame to allow systems to update
	 */
	virtual void Update( float dt ) { }

	/**
	 * Called after Update, before the render pipeline runs
	 */
	virtual void Render( ) { }

	/**
	 * Called at the end of the frame before frame capping
	 */
	virtual void EndFrame( ) { }

protected:
	
	/**
	* Initialise the system, called after settings are loaded
	*/
	virtual InitResult OnInitialize( ) { return InitResult::Initialized; }

	/**
	* Shutdown the system
	*/
	virtual void OnShutdown( ) { }

private:

	AXString mName = "Undefined";

	State mState = State::Uninitialized;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class T >
class AXSystem : public AXParent< AXSystemBase, AXSystem< T > >
{
public:
	AXSystem() : AXParent( sSystemName ) { }

	static const AXString& StaticName( ) { return sSystemName; }

	/**
	 * Returns a pointer to this system if found within the system owner
	 */
	 static const T* GetFrom( const class AXISystemOwner& sysOwner );
	 
	 /**
	 * Returns a pointer to this system if found within the system owner
	 */
	 static T* GetFrom( class AXISystemOwner& sysOwner );

protected:
	static AXString sSystemName;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXISystemOwner : public AXInterface< AXISystemOwner >
{
public:
	/**
	 * Creates a new system, returning a pointer to it
	 */
	template< class T >
	T* CreateSystem( );

	/**
	 * Attempts to find a system, if registered
	 */
	template< class T >
	const T* FindSystem( ) const;

	/**
	 * Attempts to find a system, if registered
	 */
	template< class T >
	T* FindSystem( );

	/**
	* Attempts to find a system, if registered
	*/
	const AXSystemBase* FindSystem( const AXString& sysName ) const;

	/**
	* Attempts to find a system, if registered
	*/
	AXSystemBase* FindSystem( const AXString& sysName );

protected:
	/**
	 * Returns all the systems
	 */
	const auto& GetSystems( ) const { return mSystems; }

	/**
	 * Returns all the systems
	 */
	auto& GetSystems( ) { return mSystems; }

	/**
	 * Deletes all systems
	 */
	void CleanupSystems( );

private:
	/**
	 * The owned systems
	 */
	std::vector< AXSystemBase* > mSystems;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Creates a new system, returning a pointer to it
*/
template< class T >
inline T* AXISystemOwner::CreateSystem( )
{
	static_assert( std::is_base_of< AXSystem< T >, T >::value, "T Must be a system" );

	if( T* existingSystem = FindSystem< T >() )
	{
		return existingSystem;
	}

	T* newSystem = new T( );
	mSystems.push_back( newSystem );

	return newSystem;
}

/**
* Attempts to find a system, if registered
*/
template< class T >
inline const T* AXISystemOwner::FindSystem( ) const
{
	static_assert( std::is_base_of< AXSystem< T >, T >::value, "T Must be a system" );
	
	return static_cast< const T* >( FindSystem( T::StaticName( ) ) );
}

/**
* Attempts to find a system, if registered
*/
template< class T >
inline T* AXISystemOwner::FindSystem( )
{
	static_assert( std::is_base_of< AXSystem< T >, T >::value, "T Must be a system" );

	return static_cast< T* >( FindSystem( T::StaticName( ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Returns a pointer to this system if found within the application
*/
template< class T >
const T* AXSystem< T >::GetFrom( const AXISystemOwner& sysOwner )
{
	static_assert( std::is_base_of( AXSystem< T >, T ), "T Must be a system" );
	return sysOwner.FindSystem< T >( );
}

/**
* Returns a pointer to this system if found within the application
*/
template< class T >
T* AXSystem< T >::GetFrom( AXISystemOwner& sysOwner )
{
	static_assert( std::is_base_of< AXSystem< T >, T >::value, "T Must be a system" );
	return sysOwner.FindSystem< T >( );
}