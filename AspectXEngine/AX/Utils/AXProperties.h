// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include <list>
#include <vector>
#include "AXJSON.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AX/Utils/AXPropertyMetaData.h"
#include "AXInterface.h"

// Todo, can I implement this without the virtuals?
class AXPropertyBase
{
public:
	/**
	 * Returns the name of this property
	 */
	virtual const AXString& GetName( ) = 0;

	/**
	* Writes the property into a JSON structure
	*/
	virtual void WriteToJSON( cJSON& jsonRoot ) const { }

	/**
	* Reads the property from a JSON structure
	*/
	virtual void ReadFromJSON( cJSON& jsonRoot ) { }

	/**
	 * Renders the property into the active im gui context
	 */
	virtual void RenderToImGui( ) { }
};

template< class T >
class AXProperty : public AXPropertyBase
{
public:
	using BaseType = T;
	using PropType = AXProperty< T >;
	using MetaType = AXPropertyMetaData< T >;

public:
	AXProperty( const BaseType& val ) : mVal( val ) { }

	const BaseType& Val( ) const { return mVal; }
	BaseType& Val( ) { return mVal; }

	/**
	* Returns the name of this property
	*/
	const AXString& GetName( ) override { return mMetaData.GetName( ); }

	MetaType& GetMeta( ) { return mMetaData; }
	const MetaType& GetMeta( ) const { return mMetaData; }

	/**
	* Writes the property into a JSON structure
	*/
	virtual void WriteToJSON( cJSON& jsonRoot ) const override
	{
		AXJSON::WriteValue( jsonRoot, mVal, mMetaData.GetName().c_str( ) );
	}

	/**
	* Reads the property from a JSON structure
	*/
	virtual void ReadFromJSON( cJSON& jsonRoot ) override
	{
		AXJSON::ReadValue( jsonRoot, mVal, mMetaData.GetName( ).c_str( ) );
	}

	/**
	* Renders the property into the active im gui context
	*/
	virtual void RenderToImGui( )
	{
		AXImGui::RenderPropertyToImGuiHelper( mVal, mMetaData );
	}

	// Operators

	// Cast
	
	template< class U >
	operator U( ) const { return static_cast< U >( mVal ); }

	operator bool( ) const { return static_cast< bool >( mVal ); }

	BaseType* operator -> ( ) { return &mVal; }
	const BaseType* operator -> ( ) const { return &mVal; }

	// Assignment

	template< class U >
	PropType& operator = ( const U& val ) { mVal = val; return *this; }

	template< class U >
	PropType& operator += ( const U& rhs ) { mVal += static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator -= ( const U& rhs ) { mVal -= static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator *= ( const U& rhs ) { mVal *= static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator /= ( const U& rhs ) { mVal /= static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator %= ( const U& rhs ) { mVal %= static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator &= ( const U& rhs ) { mVal &= static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator |= ( const U& rhs ) { mVal |= static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator <<= ( const U& rhs ) { mVal <<= static_cast< BaseType >( rhs ); return *this; }

	template< class U >
	PropType& operator >>= ( const U& rhs ) { mVal >>= static_cast< BaseType >( rhs ); return *this; }

	// Increment / decrement

	PropType& operator ++ ( ) { ++mVal; return *this; }

	PropType operator ++ ( int ) { T tmp( mVal ); operator++(); return tmp; }

	PropType& operator -- ( ) { --mVal; return *this; }

	PropType operator -- ( int ) { T tmp( mVal ); operator--( ); return tmp; }

	// Arithmetic

	PropType operator + ( ) const { return ( +mVal ); }

	PropType operator - ( ) const { return ( -mVal ); }

	template< class U >
	PropType operator + ( const U& rhs ) const { return ( mVal + static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator - ( const U& rhs ) const { return ( mVal - static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator * ( const U& rhs ) const { return ( mVal * static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator / ( const U& rhs ) const { return ( mVal / static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator % ( const U& rhs ) const { return ( mVal % static_cast< BaseType >( rhs ) ); }

	PropType operator ~ ( ) const { return ( ~mVal ); }

	template< class U >
	PropType operator & ( const U& rhs ) const { return ( mVal & static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator | ( const U& rhs ) const { return ( mVal | static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator ^ ( const U& rhs ) const { return ( mVal ^ static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator << ( const U& rhs ) const { return ( mVal << static_cast< BaseType >( rhs ) ); }

	template< class U >
	PropType operator >> ( const U& rhs ) const { return ( mVal >> static_cast< BaseType >( rhs ) ); }

	// Comparison

	template< class U >
	bool operator == ( const U& rhs ) const { return ( mVal == static_cast< BaseType >( rhs ) ); }

	template< class U >
	bool operator != ( const U& rhs ) const { return ( mVal != static_cast< BaseType >( rhs ) ); }

	template< class U >
	bool operator < ( const U& rhs ) const { return ( mVal < static_cast< BaseType >( rhs ) ); }

	template< class U >
	bool operator > ( const U& rhs ) const { return ( mVal > static_cast< BaseType >( rhs ) ); }

	template< class U >
	bool operator <= ( const U& rhs ) const { return ( mVal <= static_cast< BaseType >( rhs ) ); }

	template< class U >
	bool operator >= ( const U& rhs ) const { return ( mVal >= static_cast< BaseType >( rhs ) ); }

	friend class AXIPropertyHolder;

private:
	/**
	 * The variable we are masking
	 */
	BaseType mVal;

	/**
	 * Meta data about this property
	 */
	MetaType mMetaData;
};

template< class U, class V >
bool operator < ( const U& lhs, const AXProperty< V >& rhs ) { return ( static_cast< V >( lhs ) < rhs.Val() ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AXIPropertyHolder : public AXInterface< AXIPropertyHolder >
{
public:

	/**
	 * Destructor
	 */
	~AXIPropertyHolder( );

	/**
	 * Writes all properties into a JSON structure
	 */
	void WritePropertiesToJSON( cJSON& jsonRoot ) const;

	/**
	 * Reads all the properties out of a JSON structure
	 */
	void ReadPropertiesFromJSON( cJSON& jsonRoot );

	/**
	 * Renders all appropriate properties into the current ImGuiContext
	 */
	void RenderPropertiesToImGui( );

protected:
	/**
	 * Registers a property with the holder
	 */
	template< class T >
	AXPropertyMetaData< T >& RegisterProperty( AXProperty< T >& prop, const AXString& name )
	{
		prop.mMetaData.mName = name;
		mProperties.push_back( &prop );

		return prop.mMetaData;
	}

private:
	/**
	 * All the properties within this property holder
	 */
	std::vector< AXPropertyBase* > mProperties;
};