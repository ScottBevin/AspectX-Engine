// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include <functional>

#include "AX/Core/AXSystem.h"
#include "Libs/IMGui/imgui.h"
#include "AX/Utils/AXPropertyMetaData.h"
#include "AX/Utils/AXInterface.h"
#include "AX/Utils/AXParent.h"

class AXIImGuiRenderable : AXInterface< AXIImGuiRenderable >
{
public:
	virtual void RenderToImGui( const AXString& label ) = 0;
};

class AXImGui : public AXParent< AXSystem< AXImGui >, AXImGui >
{
public:

	class SystemDebugMenuItem
	{
	public:

		typedef std::map< AXString, SystemDebugMenuItem* > Collection;
		typedef std::function< void( SystemDebugMenuItem& ) > CallbackFunction;


	public:
		AXString mText;
		CallbackFunction mCallbackFunction;
		Collection mChildItems;
	};

	SystemDebugMenuItem::Collection mSystemMenuItems;

public:
	/**
	* Constructor
	*/
	AXImGui();

	/**
	* Destructor
	*/
	~AXImGui();

	/**
	 * Registers a menu to appear in the top system menu bar
	 */
	void RegisterSystemDebugMenuItem( const AXString& path, SystemDebugMenuItem::CallbackFunction callbackFunction );

	template< class T >
	static void RenderPropertyToImGuiHelper( T& val, const class AXPropertyMetaData< T >& metaData );

	template< class T >
	static void RenderPropertyToImGuiHelper( T& val, const AXString& label );

protected:

	/**
	* Initialise the system, called after settings are loaded
	*/
	virtual InitResult OnInitialize( ) override;

	/**
	* Shutdown the system
	*/
	virtual void OnShutdown( ) override;

	/**
	* Called at the start of a new frame
	*/
	virtual void BeginFrame( ) override;

	/**
	* Called once a frame to allow systems to update
	*/
	virtual void Update( float dt ) override;

	/**
	* Called after Update, before the render pipeline runs
	*/
	virtual void Render( ) override;

private:

	/**
	 * Renders out the system debug menu
	 */
	void RenderSystemDebugMenu( );

	/**
	* Renders out the system debug menu
	*/
	void RenderSystemDebugMenuRecursive( SystemDebugMenuItem::Collection& collection );

	/**
	* A callback function to draw the imgui demo window
	*/
	void ImGuiSystemDebugMenu_DemoWindowCallback( SystemDebugMenuItem& item );

	/**
	* A callback function to draw the imgui style window
	*/
	void ImGuiSystemDebugMenu_StyleWindowCallback( SystemDebugMenuItem& item );

	/**
	* A callback function to draw the imgui metrics window
	*/
	void ImGuiSystemDebugMenu_MetricsWindowCallback( SystemDebugMenuItem& item );

	/**
	* A callback function to draw the imgui user guide window
	*/
	void ImGuiSystemDebugMenu_UserGuideWindowCallback( SystemDebugMenuItem& item );

	template< class T >
	static void DoRenderPropertyToImGuiHelper( T& val );
	static void DoRenderPropertyToImGuiHelper( AXString& val );
	static void DoRenderPropertyToImGuiHelper( int& val );
	static void DoRenderPropertyToImGuiHelper( uint8_t& val );
	static void DoRenderPropertyToImGuiHelper( uint32_t& val );
	static void DoRenderPropertyToImGuiHelper( bool& val );
	static void DoRenderPropertyToImGuiHelper( float& val );

private:

	bool mShouldRenderImGuiDemoWindow = false;
	bool mShouldRenderImGuiStyleWindow = false;
	bool mShouldRenderImGuiMetricsWindow = false;
	bool mShouldRenderImGuiUserGuideWindow = false;

private:
	// Hacky stuff

	struct ID3D11Buffer*            g_pVB = NULL;
	struct ID3D11Buffer*            g_pIB = NULL;
	struct ID3D10Blob *             g_pVertexShaderBlob = NULL;
	struct ID3D11VertexShader*      g_pVertexShader = NULL;
	struct ID3D11InputLayout*       g_pInputLayout = NULL;
	struct ID3D11Buffer*            g_pVertexConstantBuffer = NULL;
	struct ID3D10Blob *             g_pPixelShaderBlob = NULL;
	struct ID3D11PixelShader*       g_pPixelShader = NULL;
	struct ID3D11SamplerState*      g_pFontSampler = NULL;
	struct ID3D11ShaderResourceView*g_pFontTextureView = NULL;
	struct ID3D11RasterizerState*   g_pRasterizerState = NULL;
	struct ID3D11BlendState*        g_pBlendState = NULL;
	struct ID3D11DepthStencilState* g_pDepthStencilState = NULL;
	int								g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class T >
bool ImGui_PropertyGetter_DropDownOptions( void* data, int idx, const char** out_text )
{
	if( out_text )
	{
		*out_text = (( *static_cast< AXPropertyMetaData< T >::DropDownOptionsCollection* >( data ) )[idx].second).c_str();
	}

	return true;
}

template< class T >
void AXImGui::RenderPropertyToImGuiHelper( T& val, const class AXPropertyMetaData< T >& metaData )
{
	if( !metaData.GetDropDownOptions( ).empty( ) )
	{
		int currentIdx( 0 );
		
		for( auto it : metaData.GetDropDownOptions() )
		{
			if( it.first == val )
			{
				break;
			}

			++currentIdx;
		}

		if( currentIdx >= ( int )metaData.GetDropDownOptions( ).size( ) )
		{
			currentIdx = 0;
		}

		ImGui::Combo( "", &currentIdx, &(ImGui_PropertyGetter_DropDownOptions< T >),
			const_cast< void* >( static_cast< const void* >( &metaData.GetDropDownOptions( ) ) ), (int)metaData.GetDropDownOptions().size() );

		val = metaData.GetDropDownOptions( )[currentIdx].first;
	}
	else
	{
		RenderPropertyToImGuiHelper( val, metaData.GetName() );
	}
}

template< class T >
void AXImGui::RenderPropertyToImGuiHelper( T& val, const AXString& label )
{
	if( std::is_base_of< AXIImGuiRenderable, T >::value )
	{
		((AXIImGuiRenderable&)( val )).RenderToImGui( label );
	}
	else
	{
		DoRenderPropertyToImGuiHelper( val );
	}
}

template< class T >
void AXImGui::DoRenderPropertyToImGuiHelper( T& val )
{
	ImGui::Text( "Unsupported Type" );
}