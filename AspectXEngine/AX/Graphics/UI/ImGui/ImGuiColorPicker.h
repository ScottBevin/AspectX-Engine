// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXColor.h"
#include "Libs/IMGui/imgui_internal.h"

namespace ImGui
{
	/**
	 * Do not use, for internal use only
	 */
	template< class TBaseType >
	bool ColorPickerWindow( const char* label, TBaseType *col, uint32_t numElements, bool* windowVisible = nullptr )
	{
		bool retVal = false;

		ImGui::SetNextWindowContentSize( ImVec2( 256.0f, 0.0f ) );
		
		if( ImGui::Begin( AXUtils::FormatString( "Color Picker: %s", label ).c_str(), windowVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize ) )
		{
			bool alphabar = ( numElements == 4 );

			float  huePickerWidth = 20.f;
			float  Spacing = ImGui::GetStyle( ).ItemInnerSpacing.x;
			float  edgeSize = ( ImGui::GetWindowWidth( ) - (huePickerWidth + Spacing) - ( ImGui::GetStyle( ).WindowPadding.x * 2.0f ) );
			
			if( alphabar )
			{
				edgeSize -= ( huePickerWidth + Spacing );
			}

			ImVec2 SV_PICKER_SIZE = ImVec2( edgeSize, edgeSize );
			float  CROSSHAIR_SIZE = 7.0f;

			ImColor color( col[0], col[1], col[2] );
			bool value_changed = false;

			ImDrawList* draw_list = ImGui::GetWindowDrawList( );

			// setup

			ImVec2 picker_pos = ImGui::GetCursorScreenPos( );

			float hue, saturation, value;
			ImGui::ColorConvertRGBtoHSV(
				color.Value.x, color.Value.y, color.Value.z, hue, saturation, value );

			// draw hue bar

			ImColor colors[] = { ImColor( 255, 0, 0 ),
				ImColor( 255, 255, 0 ),
				ImColor( 0, 255, 0 ),
				ImColor( 0, 255, 255 ),
				ImColor( 0, 0, 255 ),
				ImColor( 255, 0, 255 ),
				ImColor( 255, 0, 0 ) };

			for( int i = 0; i < 6; ++i )
			{
				draw_list->AddRectFilledMultiColor(
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x + Spacing, picker_pos.y + i * ( SV_PICKER_SIZE.y / 6 ) ),
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x + Spacing + huePickerWidth,
						picker_pos.y + ( i + 1 ) * ( SV_PICKER_SIZE.y / 6 ) ),
					colors[i],
					colors[i],
					colors[i + 1],
					colors[i + 1] );
			}

			draw_list->AddLine(
				ImVec2( picker_pos.x + SV_PICKER_SIZE.x + Spacing - 2, picker_pos.y + hue * SV_PICKER_SIZE.y ),
				ImVec2( picker_pos.x + SV_PICKER_SIZE.x + Spacing + 2 + huePickerWidth, picker_pos.y + hue * SV_PICKER_SIZE.y ),
				ImColor( 255, 255, 255 ) );

			// draw alpha bar

			if( alphabar )
			{
				float alpha = col[3];

				draw_list->AddRectFilledMultiColor(
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * Spacing + huePickerWidth, picker_pos.y ),
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * Spacing + 2 * huePickerWidth, picker_pos.y + SV_PICKER_SIZE.y ),
					ImColor( 0, 0, 0 ), ImColor( 0, 0, 0 ), ImColor( 255, 255, 255 ), ImColor( 255, 255, 255 ) );

				draw_list->AddLine(
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * ( Spacing - 2 ) + huePickerWidth, picker_pos.y + alpha * SV_PICKER_SIZE.y ),
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * ( Spacing + 2 ) + 2 * huePickerWidth, picker_pos.y + alpha * SV_PICKER_SIZE.y ),
					ImColor( 255.f - alpha, 255.f, 255.f ) );
			}

			// draw color matrix

			{
				const ImU32 c_oColorBlack = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.f, 0.f, 0.f, 1.f ) );
				const ImU32 c_oColorBlackTransparent = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.f, 0.f, 0.f, 0.f ) );
				const ImU32 c_oColorWhite = ImGui::ColorConvertFloat4ToU32( ImVec4( 1.f, 1.f, 1.f, 1.f ) );

				ImVec4 cHueValue( 1, 1, 1, 1 );
				ImGui::ColorConvertHSVtoRGB( hue, 1, 1, cHueValue.x, cHueValue.y, cHueValue.z );
				ImU32 oHueColor = ImGui::ColorConvertFloat4ToU32( cHueValue );

				draw_list->AddRectFilledMultiColor(
					ImVec2( picker_pos.x, picker_pos.y ),
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y ),
					c_oColorWhite,
					oHueColor,
					oHueColor,
					c_oColorWhite
					);

				draw_list->AddRectFilledMultiColor(
					ImVec2( picker_pos.x, picker_pos.y ),
					ImVec2( picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y ),
					c_oColorBlackTransparent,
					c_oColorBlackTransparent,
					c_oColorBlack,
					c_oColorBlack
					);
			}

			// draw cross-hair

			float x = saturation * SV_PICKER_SIZE.x;
			float y = ( 1 - value ) * SV_PICKER_SIZE.y;
			ImVec2 p( picker_pos.x + x, picker_pos.y + y );
			draw_list->AddLine( ImVec2( p.x - CROSSHAIR_SIZE, p.y ), ImVec2( p.x - 2, p.y ), ImColor( 255, 255, 255 ) );
			draw_list->AddLine( ImVec2( p.x + CROSSHAIR_SIZE, p.y ), ImVec2( p.x + 2, p.y ), ImColor( 255, 255, 255 ) );
			draw_list->AddLine( ImVec2( p.x, p.y + CROSSHAIR_SIZE ), ImVec2( p.x, p.y + 2 ), ImColor( 255, 255, 255 ) );
			draw_list->AddLine( ImVec2( p.x, p.y - CROSSHAIR_SIZE ), ImVec2( p.x, p.y - 2 ), ImColor( 255, 255, 255 ) );

			// color matrix logic

			ImGui::InvisibleButton( "saturation_value_selector", SV_PICKER_SIZE );

			if( ImGui::IsItemActive( ) && ImGui::GetIO( ).MouseDown[0] )
			{
				ImVec2 mouse_pos_in_canvas = ImVec2(
					ImGui::GetIO( ).MousePos.x - picker_pos.x, ImGui::GetIO( ).MousePos.y - picker_pos.y );

				/**/ if( mouse_pos_in_canvas.x < 0 ) mouse_pos_in_canvas.x = 0;
				else if( mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1 ) mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

				/**/ if( mouse_pos_in_canvas.y < 0 ) mouse_pos_in_canvas.y = 0;
				else if( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1 ) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

				value = 1 - ( mouse_pos_in_canvas.y / ( SV_PICKER_SIZE.y - 1 ) );
				saturation = mouse_pos_in_canvas.x / ( SV_PICKER_SIZE.x - 1 );
				value_changed = true;
			}

			// hue bar logic

			ImGui::SetCursorScreenPos( ImVec2( picker_pos.x + Spacing + SV_PICKER_SIZE.x, picker_pos.y ) );
			ImGui::InvisibleButton( "hue_selector", ImVec2( huePickerWidth, SV_PICKER_SIZE.y ) );

			if( ImGui::GetIO( ).MouseDown[0] && ( ImGui::IsItemHovered( ) || ImGui::IsItemActive( ) ) )
			{
				ImVec2 mouse_pos_in_canvas = ImVec2(
					ImGui::GetIO( ).MousePos.x - picker_pos.x, ImGui::GetIO( ).MousePos.y - picker_pos.y );

				/**/ if( mouse_pos_in_canvas.y < 0 ) mouse_pos_in_canvas.y = 0;
				else if( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1 ) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

				hue = mouse_pos_in_canvas.y / ( SV_PICKER_SIZE.y - 1 );
				value_changed = true;
			}

			// alpha bar logic

			if( alphabar )
			{

				ImGui::SetCursorScreenPos( ImVec2( picker_pos.x + Spacing * 2 + huePickerWidth + SV_PICKER_SIZE.x, picker_pos.y ) );
				ImGui::InvisibleButton( "alpha_selector", ImVec2( huePickerWidth, SV_PICKER_SIZE.y ) );

				if( ImGui::GetIO( ).MouseDown[0] && ( ImGui::IsItemHovered( ) || ImGui::IsItemActive( ) ) )
				{
					ImVec2 mouse_pos_in_canvas = ImVec2(
						ImGui::GetIO( ).MousePos.x - picker_pos.x, ImGui::GetIO( ).MousePos.y - picker_pos.y );

					/**/ if( mouse_pos_in_canvas.y < 0 ) mouse_pos_in_canvas.y = 0;
					else if( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1 ) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

					float alpha = mouse_pos_in_canvas.y / ( SV_PICKER_SIZE.y - 1 );
					col[3] = alpha;
					value_changed = true;
				}

			}

			// R,G,B or H,S,V color editor

			color = ImColor::HSV( ( float )( hue >= 1 ? hue - 10 * 1e-6 : hue ), ( float )( saturation > 0 ? saturation : 10 * 1e-6 ), ( float )( value > 0 ? value : 1e-6 ) );
			col[0] = color.Value.x;
			col[1] = color.Value.y;
			col[2] = color.Value.z;

			bool widget_used = false;
 			ImGui::PushItemWidth( ImGui::GetWindowWidth( ) - ( ImGui::GetStyle().DisplayWindowPadding.x * 2.0f ) );
 			widget_used = alphabar ? ImGui::ColorEdit4( "", col ) : ImGui::ColorEdit3( "", col );
 			ImGui::PopItemWidth( );

			// try to cancel hue wrap (after ColorEdit), if any
			{
				float new_hue, new_sat, new_val;
				ImGui::ColorConvertRGBtoHSV( col[0], col[1], col[2], new_hue, new_sat, new_val );
				if( new_hue <= 0 && hue > 0 )
				{
					if( new_val <= 0 && value != new_val )
					{
						color = ImColor::HSV( hue, saturation, new_val <= 0 ? value * 0.5f : new_val );
						col[0] = color.Value.x;
						col[1] = color.Value.y;
						col[2] = color.Value.z;
					}
					else
						if( new_sat <= 0 )
						{
							color = ImColor::HSV( hue, new_sat <= 0 ? saturation * 0.5f : new_sat, new_val );
							col[0] = color.Value.x;
							col[1] = color.Value.y;
							col[2] = color.Value.z;
						}
				}
			}

			retVal = ( value_changed | widget_used );
		}

		ImGui::End( );

		return retVal;
	}

	template< class TBaseType >
	void ColorPickerPreview_Impl( const char* label, TBaseType *color, uint32_t numElements, int line, bool& windowVisible, bool showLabel )
	{
		if( numElements < 3 )
		{
			return;
		}

		if( ImGuiContext* g = ImGui::GetCurrentContext( ) )
		{
			if( ImGuiWindow* window = GetCurrentWindow( ) )
			{
				if( showLabel )
				{
					ImGui::Text( label );

					ImGui::SameLine( );
				}

				ImVec4 col( color[ 0 ], color[1], color[2], numElements == 4 ? color[3] : 1.0f );

				const ImGuiStyle& style = g->Style;
				const ImGuiID id = window->GetID( AXUtils::FormatString( "#ColorPickerPreview: %s %d", label, line ).c_str() );
				const float square_size = g->FontSize;
				ImRect bb;
				bb.Min = bb.Max = window->DC.CursorPos;
				bb.Max.x += window->ItemWidthDefault;
				bb.Max.y += square_size;

				ItemSize( bb, 0.0f );

				if( !ItemAdd( bb, &id ) )
					return;

				bool hovered, held;
				bool pressed = ButtonBehavior( bb, id, &hovered, &held );

				RenderFrame( bb.Min, bb.Max, GetColorU32( col ), false, style.FrameRounding );

				if( hovered )
					SetTooltip( "Color:\n(%.2f,%.2f,%.2f,%.2f)\n#%02X%02X%02X%02X", col.x, col.y, col.z, col.w, IM_F32_TO_INT8_SAT( col.x ), IM_F32_TO_INT8_SAT( col.y ), IM_F32_TO_INT8_SAT( col.z ), IM_F32_TO_INT8_SAT( col.z ) );

				if( pressed )
				{
					windowVisible = !windowVisible;
				}

				if( windowVisible )
				{
					ColorPickerWindow( label, color, numElements, &windowVisible );
				}
			}
		}
	}

#define ImGuiColorPickerPreview( LABEL, COL, NUM_ELEMENTS, SHOW_LABEL ) do { static bool AXJOIN( WindowVisible, __LINE__ ) = false; ImGui::ColorPickerPreview_Impl( (LABEL), (COL), (NUM_ELEMENTS), __LINE__, AXJOIN( WindowVisible, __LINE__ ), (SHOW_LABEL) ); } while( false )
};