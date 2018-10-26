// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"
#include "AX/Utils/AXString.h"
#include <vector>

class AXDirectory : AXParent< AXBaseObject, AXDirectory >
{
public:
	struct Item
	{
	public:
		struct Type
		{
			enum E
			{
				Directory,
				File,
			};
		};

		/**
		 * The type of this item
		 */
		Type::E mType;

		/**
		 * This items name
		 */
		AXString mName;
	};

	using DirectoryContents = std::vector< Item >;

public:
	/**
	 * Clears out the current directory results and reads the results in from the new path, returns true if the directory exists
	 */
	bool Open( const AXString& path );
	
	/**
	 * Sets up a watch on this directory, auto-refreshes if the directory changes
	 */
	void WatchForChanges( );

	/**
	 * Returns the path to the opened directory
	 */
	const AXString& GetPath( ) const { return mPath; }

	/**
	 * Returns the contents gathered from this directory
	 */
	const DirectoryContents& GetContents( ) const { return mContents; }

	/**
	 * Returns true if this directory exists
	 */
	static bool Exists( const AXString& path );

private:
	/**
	 * All items within the directory
	 */
	DirectoryContents mContents;
	
	/**
	 * The path to this directory
	 */
	AXString mPath = "";
};