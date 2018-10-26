// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include <d3d11.h>

#include "AX/Utils/AXParent.h"
#include "AX/Content/AXAsset.h"

class AXTexture : public AXParent< AXAsset< ID3D11Texture2D >, AXTexture >
{
public:

};