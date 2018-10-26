// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

class AXInterfaceBase
{
public:

};

template< class T >
class AXInterface : public AXInterfaceBase
{
public:

};

template< class T >
class AXImplementsInterface : public T
{
	static_assert( std::is_base_of< AXInterface< T >, T >::value, "T must be an AX interface" );

public:

};
