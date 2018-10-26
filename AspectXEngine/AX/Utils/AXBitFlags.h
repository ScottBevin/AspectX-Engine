#pragma once

/**
 * A simple helper utility for managing bit flags, usage is to pass a template struct in containing an enum "E" which
 * defined the enumerated flag values, E must contain FlagMax denoting the end of the flag values
 */
template< typename TEnumWrapper, typename TStorageType = uint8_t >
class AXBitFlags
{
	static_assert( TEnumWrapper::FlagMax > 0, "AXBitFlags<TEnumWrapper::FlagMax> must be > 0" );

public:

	void Set( TEnumWrapper::E flag )
	{
		AXASSERT( flag < TEnumWrapper::FlagMax, "Value out of range, Value: %u, Max: %u", flag, TEnumWrapper::FlagMax );
		mStorage[flag / sizeof( TStorageType )] &= ( 0x1 << ( flag % sizeof( TStorageType ) ) );
	}

private:
	TStorageType mStorage[ (TEnumWrapper::FlagMax + ((sizeof( TStorageType ) * 8) - 1)) / sizeof( TStorageType ))];
};
