// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Math/AXMathVector.h"
#include "AX/Graphics/UI/ImGui/ImGuiColorPicker.h"
#include "AXParent.h"

template< class TBaseType, uint32_t TNumElements >
class AXColor : public AXParent< AXMathVector< TBaseType, TNumElements >, AXColor< TBaseType, TNumElements > >
{
public:
	using AXParent< AXMathVector< TBaseType, TNumElements >, AXColor< TBaseType, TNumElements > >::AXParent;

	TBaseType& R( ) { return X( ); }
	TBaseType& G( ) { return Y( ); }
	TBaseType& B( ) { return Z( ); }
	TBaseType& A( ) { return W( ); }

	const TBaseType& R( ) const { return X( ); }
	const TBaseType& G( ) const { return Y( ); }
	const TBaseType& B( ) const { return Z( ); }
	const TBaseType& A( ) const { return W( ); }

	virtual void RenderToImGui( const AXString& label ) override;
};

using AXColorRGBf = AXColor< float, 3 >;
using AXColorRGBAf = AXColor< float, 4 >;

using AXColorRGBu8 = AXColor< uint8_t, 3 >;
using AXColorRGBAu8 = AXColor< uint8_t, 4 >;

template< class TBaseType, uint32_t TNumElements >
void AXColor< TBaseType, TNumElements >::RenderToImGui( const AXString& label )
{
	ImGuiColorPickerPreview( label.c_str(), &(mElements[0]), TNumElements, false );
}