// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXJSON.h"
#include "AX/Graphics/UI/ImGui/AXImGui.h"
#include "AX/Utils/AXInterface.h"
#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"

template< class TBaseType, uint32_t TNumElements >
class AXMathVector : public AXParent< AXBaseObject, AXMathVector< TBaseType, TNumElements > >
				, public AXImplementsInterface< AXIJSONParsable >
				, public AXImplementsInterface< AXIImGuiRenderable >
{
public:
	using VectorType = AXMathVector< TBaseType, TNumElements >;
	using BaseType = TBaseType;
	static const uint32_t NumElements = TNumElements;

public:
	
	AXMathVector( const BaseType& val )
	{
		for( auto& ele : mElements )
		{
			ele = val;
		}
	}

	template< typename... Args >
	AXMathVector( Args... args )
	{
		static_assert( sizeof...( args ) == NumElements, "AXVector constructor being passed the wrong number of values." );

		BaseType* ele( &mElements[0] );
		for( auto val : { args... } )
		{
			*( ele++ ) = static_cast< BaseType >( val );
		}
	}

	// Operators

	// Assignment

	template< class U >
	VectorType& operator = ( const AXMathVector< U, NumElements >& val )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] = static_cast< BaseType >( val.mElements[i] );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator = ( const U& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] = static_cast< BaseType >( rhs );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator += ( const AXMathVector< U, NumElements >& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] += static_cast< BaseType >( rhs.mElements[i] );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator += ( const U& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] += static_cast< BaseType >( rhs );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator -= ( const AXMathVector< U, NumElements >& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] -= static_cast< BaseType >( rhs.mElements[i] );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator -= ( const U& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] -= static_cast< BaseType >( rhs );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator *= ( const AXMathVector< U, NumElements >& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] *= static_cast< BaseType >( rhs.mElements[i] );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator *= ( const U& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] *= static_cast< BaseType >( rhs );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator /= ( const AXMathVector< U, NumElements >& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] /= static_cast< BaseType >( rhs.mElements[i] );
		}

		return ( *this );
	}

	template< class U >
	VectorType& operator /= ( const U& rhs )
	{
		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			mElements[i] /= static_cast< BaseType >( rhs );
		}

		return ( *this );
	}

	// Arithmetic

	template< class U >
	VectorType operator + ( const AXMathVector< U, NumElements >& rhs ) const
	{
		VectorType ret( *this );
		ret += rhs;
		return ret;
	}

	template< class U >
	VectorType operator + ( const U& rhs ) const
	{
		VectorType ret( *this );
		ret += rhs;
		return ret;
	}

	template< class U >
	VectorType operator - ( const AXMathVector< U, NumElements >& rhs ) const
	{
		VectorType ret( *this );
		ret -= rhs;
		return ret;
	}

	template< class U >
	VectorType operator - ( const U& rhs ) const
	{
		VectorType ret( *this );
		ret -= rhs;
		return ret;
	}

	// Comparison

	template< class T >
	bool operator == ( const AXMathVector< T, NumElements >& rhs ) const
	{
		bool isEqual( true );

		for( uint32_t i( 0 ); i < NumElements; ++i )
		{
			isEqual &= ( mElements[i] == rhs.mElements[i] );
		}

		return isEqual;
	}

	template< class U >
	bool operator != ( const AXMathVector< U, NumElements >& rhs ) const
	{
		return !( ( *this ) == rhs );
	}

	// Member access

	BaseType& X( ) { return mElements[0]; }
	BaseType& Y( ) { static_assert( NumElements >= 2, "Vector size too small." ); return mElements[1]; }
	BaseType& Z( ) { static_assert( NumElements >= 3, "Vector size too small." ); return mElements[2]; }
	BaseType& W( ) { static_assert( NumElements >= 4, "Vector size too small." ); return mElements[3]; }

	const BaseType& X( ) const { return mElements[0]; }
	const BaseType& Y( ) const { static_assert( NumElements >= 2, "Vector size too small." ); return mElements[1]; }
	const BaseType& Z( ) const { static_assert( NumElements >= 3, "Vector size too small." ); return mElements[2]; }
	const BaseType& W( ) const { static_assert( NumElements >= 4, "Vector size too small." ); return mElements[3]; }

	// Helpers

	virtual void ReadFromJSON( cJSON& jsonRoot, const char* name ) override;

	virtual void WriteToJSON( cJSON& jsonRoot, const char* name ) const override;

	virtual void RenderToImGui( const AXString& label ) override;

public:
	BaseType mElements[NumElements];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using AXVector2f = AXMathVector< float, 2 >;
using AXVector3f = AXMathVector< float, 3 >;
using AXVector4f = AXMathVector< float, 4 >;

using AXVector2d = AXMathVector< double, 2 >;
using AXVector3d = AXMathVector< double, 3 >;
using AXVector4d = AXMathVector< double, 4 >;

using AXVector2u8 = AXMathVector< uint8_t, 2 >;
using AXVector3u8 = AXMathVector< uint8_t, 3 >;
using AXVector4u8 = AXMathVector< uint8_t, 4 >;

using AXVector2u16 = AXMathVector< uint16_t, 2 >;
using AXVector3u16 = AXMathVector< uint16_t, 3 >;
using AXVector4u16 = AXMathVector< uint16_t, 4 >;

using AXVector2u32 = AXMathVector< uint32_t, 2 >;
using AXVector3u32 = AXMathVector< uint32_t, 3 >;
using AXVector4u32 = AXMathVector< uint32_t, 4 >;

using AXVector2u64 = AXMathVector< uint64_t, 2 >;
using AXVector3u64 = AXMathVector< uint64_t, 3 >;
using AXVector4u64 = AXMathVector< uint64_t, 4 >;

using AXVector2i8 = AXMathVector< int8_t, 2 >;
using AXVector3i8 = AXMathVector< int8_t, 3 >;
using AXVector4i8 = AXMathVector< int8_t, 4 >;

using AXVector2i16 = AXMathVector< int16_t, 2 >;
using AXVector3i16 = AXMathVector< int16_t, 3 >;
using AXVector4i16 = AXMathVector< int16_t, 4 >;

using AXVector2i32 = AXMathVector< int32_t, 2 >;
using AXVector3i32 = AXMathVector< int32_t, 3 >;
using AXVector4i32 = AXMathVector< int32_t, 4 >;

using AXVector2i64 = AXMathVector< int64_t, 2 >;
using AXVector3i64 = AXMathVector< int64_t, 3 >;
using AXVector4i64 = AXMathVector< int64_t, 4 >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Helpers
template< class TBaseType, uint32_t TNumElements >
void AXMathVector< TBaseType, TNumElements >::ReadFromJSON( cJSON& jsonRoot, const char* name )
{
	if( cJSON* jsonArray = cJSON_GetObjectItem( &jsonRoot, name ) )
	{
		uint32_t elementsInJsonArray( cJSON_GetArraySize( jsonArray ) );
		AXASSERT_WARN( elementsInJsonArray == NumElements, "JSON has the wrong number of elements for vector" );

		for( uint32_t i( 0 ); i < AXUtils::Min( elementsInJsonArray, TNumElements ); ++i )
		{
			AXJSON::ReadValueFromArray( *jsonArray, mElements[i], i );
		}
	}
}

template< class TBaseType, uint32_t TNumElements >
void AXMathVector< TBaseType, TNumElements >::WriteToJSON( cJSON& jsonRoot, const char* name ) const
{
	if( cJSON* jsonArray = cJSON_CreateArray( ) )
	{
		cJSON_AddItemToObject( &jsonRoot, name, jsonArray );

		for( const BaseType& val : mElements )
		{
			AXJSON::WriteValueToArray( *jsonArray, val );
		}
	}
}

template< class TBaseType, uint32_t TNumElements >
void AXMathVector< TBaseType, TNumElements >::RenderToImGui( const AXString& label )
{
	ImGui::BeginGroup( );

	ImGui::PushMultiItemsWidths( TNumElements );

	for( uint32_t i( 0 ); i < TNumElements; ++i )
	{
		ImGui::PushID( i );

		AXImGui::RenderPropertyToImGuiHelper< BaseType >( mElements[ i ], label );
		ImGui::SameLine( 0.0f, ImGui::GetStyle().ItemInnerSpacing.x );

		ImGui::PopID( );

		ImGui::PopItemWidth( );
	}

	ImGui::EndGroup( );
}