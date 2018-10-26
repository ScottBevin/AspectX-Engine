// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "Libs/cJSON/cJSON.h"
#include "AX/Utils/AXString.h"
#include "AXInterface.h"

class AXIJSONParsable : public AXInterface< AXIJSONParsable >
{
public:
	virtual void ReadFromJSON( cJSON& jsonRoot, const char* name ) = 0;
	virtual void WriteToJSON( cJSON& jsonRoot, const char* name ) const = 0;
};

class AXJSON
{
public:
	static void WriteValue( cJSON& jsonRoot, const AXIJSONParsable& val, const char* name ) { val.WriteToJSON( jsonRoot, name ); }

	static void WriteValue( cJSON& jsonRoot, const AXString& val, const char* name ) { cJSON_AddStringToObject( &jsonRoot, name, val.c_str( ) ); }

	static void WriteValue( cJSON& jsonRoot, const bool& val, const char* name ) { cJSON_AddBoolToObject( &jsonRoot, name, val ); }

	static void WriteValue( cJSON& jsonRoot, const uint8_t& val, const char* name ) { cJSON_AddNumberToObject( &jsonRoot, name, val ); }

	static void WriteValue( cJSON& jsonRoot, const uint16_t& val, const char* name ) { cJSON_AddNumberToObject( &jsonRoot, name, val ); }

	static void WriteValue( cJSON& jsonRoot, const uint32_t& val, const char* name ) { cJSON_AddNumberToObject( &jsonRoot, name, val ); }

	static void WriteValue( cJSON& jsonRoot, const uint64_t& val, const char* name ) { cJSON_AddNumberToObject( &jsonRoot, name, ( uint32_t )val ); }

	//////////////////////////////////////////////////////////////////////////

	template< class T >
	static void WriteValueToArray( cJSON& jsonRoot, const T& val ) { cJSON_AddItemToArray( &jsonRoot, cJSON_CreateNumber( val ) ); }

	static void WriteValueToArray( cJSON& jsonRoot, const AXString& val ) { cJSON_AddItemToArray( &jsonRoot, cJSON_CreateString( val.c_str( ) ) ); }

	static void WriteValueToArray( cJSON& jsonRoot, const bool& val ) { cJSON_AddItemToArray( &jsonRoot, cJSON_CreateBool( val ) ); }

	//////////////////////////////////////////////////////////////////////////

	static void ReadValue( cJSON& jsonRoot, AXIJSONParsable& val, const char* name ) { val.ReadFromJSON( jsonRoot, name ); }

	static void ReadValue( cJSON& jsonRoot, float& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = ( float )versionJSON->valuedouble;
		}
	}

	static void ReadValue( cJSON& jsonRoot, double& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = versionJSON->valuedouble;
		}
	}

	static void ReadValue( cJSON& jsonRoot, AXString& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = versionJSON->valuestring;
		}
	}

	static void ReadValue( cJSON& jsonRoot, bool& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = versionJSON->valueint != 0;
		}
	}

	static void ReadValue( cJSON& jsonRoot, uint8_t& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = ( uint8_t )versionJSON->valueint;
		}
	}

	static void ReadValue( cJSON& jsonRoot, uint16_t& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = ( uint16_t )versionJSON->valueint;
		}
	}

	static void ReadValue( cJSON& jsonRoot, uint32_t& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = ( uint32_t )versionJSON->valueint;
		}
	}

	static void ReadValue( cJSON& jsonRoot, uint64_t& val, const char* name )
	{
		if( cJSON* versionJSON = cJSON_GetObjectItem( &jsonRoot, name ) )
		{
			val = ( uint32_t )versionJSON->valueint;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	static void ReadValueFromArray( cJSON& jsonRoot, float& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( float )arrayItem->valuedouble;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, double& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = arrayItem->valuedouble;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, AXString& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = arrayItem->valuestring;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, bool& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = arrayItem->valueint != 0;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, uint8_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( uint8_t)arrayItem->valueint;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, uint16_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( uint16_t )arrayItem->valueint;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, uint32_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( uint32_t )arrayItem->valueint;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, uint64_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( uint64_t )arrayItem->valueint;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, int8_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( int8_t )arrayItem->valueint;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, int16_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( int16_t )arrayItem->valueint;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, int32_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( int32_t )arrayItem->valueint;
		}
	}

	static void ReadValueFromArray( cJSON& jsonRoot, int64_t& val, uint32_t idx )
	{
		if( cJSON* arrayItem = cJSON_GetArrayItem( &jsonRoot, idx ) )
		{
			val = ( int64_t )arrayItem->valueint;
		}
	}
};