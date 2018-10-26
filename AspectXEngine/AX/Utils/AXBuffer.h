#pragma once
#include <stdint.h>

template< typename TSize >
class AXBuffer
{
public:

	AXBuffer( )
		: mAddr( nullptr )
		, mSize( 0 )
	{

	}

	AXBuffer( TSize size )
		: mAddr( new uint8_t[size] )
		, mSize( size )
	{
	}

	~AXBuffer( )
	{
		delete mAddr;
	}

	uint8_t* mAddr;
	TSize mSize;
};

using AXBuffer8 = AXBuffer<uint8_t>;
using AXBuffer16 = AXBuffer<uint16_t>;
using AXBuffer32 = AXBuffer<uint32_t>; 
using AXBuffer64 = AXBuffer<uint64_t>;