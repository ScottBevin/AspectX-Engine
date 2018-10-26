// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

/**
* Meta data used to display things in debug builds.
* Todo: Strip this out of none debug builds
*/
template< class T >
class AXPropertyMetaData
{
public:
	using DropDownOptionsCollectionEntry = std::pair< T, AXString >;
	using DropDownOptionsCollection = std::vector< DropDownOptionsCollectionEntry >;

public:

	/**
	* If true then when the standard mechanism renders this property in ImGui it will render as a drop down
	*/
	AXPropertyMetaData< T >& DisplayAsDropDown( const DropDownOptionsCollection& options ) { mDropDownOptions = options; return ( *this ); }
	const DropDownOptionsCollection& GetDropDownOptions( ) const { return mDropDownOptions; }

	const AXString& GetName( ) const { return mName; }

	friend class AXPropertyBase;
	friend class AXIPropertyHolder;

private:
	AXString mName = "Unknown";

	/**
	* If not empty, this property will show up as a dropdown
	*/
	DropDownOptionsCollection mDropDownOptions;
};