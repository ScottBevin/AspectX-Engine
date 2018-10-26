// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"
#include "AXAssetHandle.h"

class AXAssetBase : public AXParent< AXBaseObject, AXAssetBase >
{

};

template< class TRawType >
class AXAsset : public AXParent< AXAssetBase, AXAsset< TRawType > >
{
public:

	using RawType = TRawType;
	using Handle = AXAssetHandle< AXAsset< TRawType > >;

public:


private:

};