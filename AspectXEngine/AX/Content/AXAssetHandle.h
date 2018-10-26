// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXBaseObject.h"

class AXAssetHandleBase : public AXParent< AXBaseObject, AXAssetHandleBase >
{

};

template< class TAssetType >
class AXAssetHandle : public AXParent< AXAssetHandleBase, AXAssetHandleBase >
{
public:
	using AssetType = TAssetType;
};