// Copyright 2016 Scott Bevin, All Rights Reserved

#include "AXImGui.h"

#include "AX/Core/AXWindow.h"
#include "AX/Graphics/RenderCore/AXRenderCore.h"
#include "AX/Core/AXApplication.h"
#include "AX/Graphics/RenderCore/AXGraphicsDevice.h"

// DirectX
#include <d3d11.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "AX/Utils/AXProperties.h"

AXString AXSystem< AXImGui >::sSystemName = "ImGui";

/**
* Constructor
*/
AXImGui::AXImGui()
{

}

/**
* Destructor
*/
AXImGui::~AXImGui()
{

}

struct VERTEX_CONSTANT_BUFFER
{
	float        mvp[4][4];
};

/**
* Initialise the system, called after settings are loaded
*/
AXImGui::InitResult AXImGui::OnInitialize( )
{ 
	ImGui::CreateContext(nullptr);

	AXRenderCore* renderCore( AXRenderCore::GetFrom( AXApplication::Get( ) ) );
	AXWindow* window( AXWindow::GetFrom( AXApplication::Get( ) ) );

	if( !renderCore || !window )
	{
		return AXSystem::InitResult::Failed;
	}

	if( window->GetState( ) != AXSystemBase::State::Initialized ||
		renderCore->GetState() != AXSystemBase::State::Initialized )
	{
		return AXSystemBase::InitResult::Retry;
	} 

	ImGuiIO& io = ImGui::GetIO( );
	io.DisplaySize.x = renderCore->GetSettings().mScreenWidth;
	io.DisplaySize.y = renderCore->GetSettings( ).mScreenHeight;
	io.IniFilename = "imgui.cfg";
	io.RenderDrawListsFn = nullptr;
	io.ImeWindowHandle = (void*)(&window->GetWindowHandle( ));

	// Create device objects

	// By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
	// If you would like to use this DX11 sample code but remove this dependency you can: 
	//  1) compile once, save the compiled shader blobs into a file or source code and pass them to CreateVertexShader()/CreatePixelShader() [prefered solution]
	//  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL. 
	// See https://github.com/ocornut/imgui/pull/638 for sources and details.

	// Create the vertex shader
	{
		static const char* vertexShader =
			"cbuffer vertexBuffer : register(b0) \
            {\
            float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
            float2 pos : POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
            PS_INPUT output;\
            output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
            output.col = input.col;\
            output.uv  = input.uv;\
            return output;\
            }";

		D3DCompile( vertexShader, strlen( vertexShader ), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &g_pVertexShaderBlob, NULL );
		if( g_pVertexShaderBlob == NULL ) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
			return AXSystem::InitResult::Failed;
		if( renderCore->GetGraphicsDevice().GetDXDevice().CreateVertexShader( ( DWORD* )g_pVertexShaderBlob->GetBufferPointer( ), g_pVertexShaderBlob->GetBufferSize( ), NULL, &g_pVertexShader ) != S_OK )
			return AXSystem::InitResult::Failed;

		// Create the input layout
		D3D11_INPUT_ELEMENT_DESC local_layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, ( size_t )( &( ( ImDrawVert* )0 )->pos ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, ( size_t )( &( ( ImDrawVert* )0 )->uv ),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, ( size_t )( &( ( ImDrawVert* )0 )->col ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		if( renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateInputLayout( local_layout, 3, g_pVertexShaderBlob->GetBufferPointer( ), g_pVertexShaderBlob->GetBufferSize( ), &g_pInputLayout ) != S_OK )
			return AXSystem::InitResult::Failed;

		// Create the constant buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = sizeof( VERTEX_CONSTANT_BUFFER );
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateBuffer( &desc, NULL, &g_pVertexConstantBuffer );
		}
	}

	// Create the pixel shader
	{
		static const char* pixelShader =
			"struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";

		D3DCompile( pixelShader, strlen( pixelShader ), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_pPixelShaderBlob, NULL );
		if( g_pPixelShaderBlob == NULL )  // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
			return AXSystem::InitResult::Failed;
		if( renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreatePixelShader( ( DWORD* )g_pPixelShaderBlob->GetBufferPointer( ), g_pPixelShaderBlob->GetBufferSize( ), NULL, &g_pPixelShader ) != S_OK )
			return AXSystem::InitResult::Failed;
	}

	// Create the blending setup
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateBlendState( &desc, &g_pBlendState );
	}

	// Create the rasterizer state
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.ScissorEnable = true;
		desc.DepthClipEnable = true;
		renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateRasterizerState( &desc, &g_pRasterizerState );
	}

	// Create depth-stencil State
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;
		renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateDepthStencilState( &desc, &g_pDepthStencilState );
	}

	// Create fonts texture
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );

	// Upload texture to graphics system
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D *pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = pixels;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateTexture2D( &desc, &subResource, &pTexture );

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory( &srvDesc, sizeof( srvDesc ) );
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateShaderResourceView( pTexture, &srvDesc, &g_pFontTextureView );
		pTexture->Release( );
	}

	// Store our identifier
	io.Fonts->TexID = ( void * )g_pFontTextureView;

	// Create texture sampler
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.f;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.MinLOD = 0.f;
		desc.MaxLOD = 0.f;
		renderCore->GetGraphicsDevice( ).GetDXDevice( ).CreateSamplerState( &desc, &g_pFontSampler );
	}

	RegisterSystemDebugMenuItem( "Window/ImGui/Demo", std::bind( &AXImGui::ImGuiSystemDebugMenu_DemoWindowCallback, this, std::placeholders::_1 ) );
	RegisterSystemDebugMenuItem( "Window/ImGui/Style", std::bind( &AXImGui::ImGuiSystemDebugMenu_StyleWindowCallback, this, std::placeholders::_1 ) );
	RegisterSystemDebugMenuItem( "Window/ImGui/User Guide", std::bind( &AXImGui::ImGuiSystemDebugMenu_UserGuideWindowCallback, this, std::placeholders::_1 ) );
	RegisterSystemDebugMenuItem( "Window/ImGui/Metrics", std::bind( &AXImGui::ImGuiSystemDebugMenu_MetricsWindowCallback, this, std::placeholders::_1 ) );

	return InitResult::Initialized; 
}

/**
* Shutdown the system
*/
void AXImGui::OnShutdown( )
{
	if( g_pFontSampler ) { g_pFontSampler->Release( ); g_pFontSampler = NULL; }
	if( g_pFontTextureView ) { g_pFontTextureView->Release( ); g_pFontTextureView = NULL; ImGui::GetIO( ).Fonts->TexID = 0; }
	if( g_pIB ) { g_pIB->Release( ); g_pIB = NULL; }
	if( g_pVB ) { g_pVB->Release( ); g_pVB = NULL; }

	if( g_pBlendState ) { g_pBlendState->Release( ); g_pBlendState = NULL; }
	if( g_pDepthStencilState ) { g_pDepthStencilState->Release( ); g_pDepthStencilState = NULL; }
	if( g_pRasterizerState ) { g_pRasterizerState->Release( ); g_pRasterizerState = NULL; }
	if( g_pPixelShader ) { g_pPixelShader->Release( ); g_pPixelShader = NULL; }
	if( g_pPixelShaderBlob ) { g_pPixelShaderBlob->Release( ); g_pPixelShaderBlob = NULL; }
	if( g_pVertexConstantBuffer ) { g_pVertexConstantBuffer->Release( ); g_pVertexConstantBuffer = NULL; }
	if( g_pInputLayout ) { g_pInputLayout->Release( ); g_pInputLayout = NULL; }
	if( g_pVertexShader ) { g_pVertexShader->Release( ); g_pVertexShader = NULL; }
	if( g_pVertexShaderBlob ) { g_pVertexShaderBlob->Release( ); g_pVertexShaderBlob = NULL; }

	ImGui::Shutdown(ImGui::GetCurrentContext());
}

/**
* Called at the start of a new frame
*/
void AXImGui::BeginFrame( )
{
	ImGuiIO& io = ImGui::GetIO( );

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	GetClientRect( static_cast< HWND >(&AXWindow::GetFrom( AXApplication::Get() )->GetWindowHandle()), &rect );
	io.DisplaySize = ImVec2( ( float )( rect.right - rect.left ), ( float )( rect.bottom - rect.top ) );

	// Read keyboard modifiers inputs
	io.KeyCtrl = ( GetKeyState( VK_CONTROL ) & 0x8000 ) != 0;
	io.KeyShift = ( GetKeyState( VK_SHIFT ) & 0x8000 ) != 0;
	io.KeyAlt = ( GetKeyState( VK_MENU ) & 0x8000 ) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	SetCursor( io.MouseDrawCursor ? NULL : LoadCursor( NULL, IDC_ARROW ) );

	// Start the frame
	ImGui::NewFrame( );
}

/**
* Called once a frame to allow systems to update
*/
void AXImGui::Update( float dt )
{
	ImGuiIO& io = ImGui::GetIO( );

	io.DeltaTime = dt;

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	GetClientRect( static_cast< HWND >( &AXWindow::GetFrom( AXApplication::Get( ) )->GetWindowHandle( ) ), &rect );
	io.DisplaySize.x = ( float )( rect.right - rect.left );
	io.DisplaySize.y = ( float )( rect.bottom - rect.top );

	io.KeyCtrl = ( GetKeyState( VK_CONTROL ) & 0x8000 ) != 0;
	io.KeyShift = ( GetKeyState( VK_SHIFT ) & 0x8000 ) != 0;
	io.KeyAlt = ( GetKeyState( VK_MENU ) & 0x8000 ) != 0;
	io.KeySuper = false;
}

/**
* Called after Update, before the render pipeline runs
*/
void AXImGui::Render( )
{
	RenderSystemDebugMenu( );

	if( mShouldRenderImGuiDemoWindow )
	{
		//ImGui::ShowTestWindow( &mShouldRenderImGuiDemoWindow );
	}

	if( mShouldRenderImGuiMetricsWindow )
	{
		ImGui::ShowMetricsWindow( &mShouldRenderImGuiMetricsWindow );
	}

	if( mShouldRenderImGuiStyleWindow )
	{
		if( ImGui::Begin( "ImGui Style", &mShouldRenderImGuiStyleWindow ) )
		{
			ImGui::ShowStyleEditor( );
		}

		ImGui::End( );
	}

	if( mShouldRenderImGuiUserGuideWindow )
	{
		if( ImGui::Begin( "ImGui User Guide", &mShouldRenderImGuiUserGuideWindow ) )
		{
			ImGui::ShowUserGuide( );
		}

		ImGui::End( );
	}

	//////////////////////////////////////////////////////////////////////////

	ImGui::Render( );

	// Hacky stuff

	if( ImDrawData* draw_data = ImGui::GetDrawData( ) )
	{
		ID3D11DeviceContext* ctx = &AXRenderCore::GetFrom( AXApplication::Get( ) )->GetGraphicsDevice( ).GetDXDeviceContext( );
		ID3D11Device* g_pd3dDevice = &AXRenderCore::GetFrom( AXApplication::Get( ) )->GetGraphicsDevice( ).GetDXDevice( );

		// Create and grow vertex/index buffers if needed
		if( !g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount )
		{
			if( g_pVB ) { g_pVB->Release( ); g_pVB = NULL; }
			g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
			D3D11_BUFFER_DESC desc;
			memset( &desc, 0, sizeof( D3D11_BUFFER_DESC ) );
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = g_VertexBufferSize * sizeof( ImDrawVert );
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			if( g_pd3dDevice->CreateBuffer( &desc, NULL, &g_pVB ) < 0 )
				return;
		}
		if( !g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount )
		{
			if( g_pIB ) { g_pIB->Release( ); g_pIB = NULL; }
			g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
			D3D11_BUFFER_DESC desc;
			memset( &desc, 0, sizeof( D3D11_BUFFER_DESC ) );
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = g_IndexBufferSize * sizeof( ImDrawIdx );
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			if( g_pd3dDevice->CreateBuffer( &desc, NULL, &g_pIB ) < 0 )
				return;
		}

		// Copy and convert all vertices into a single contiguous buffer
		D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
		if( ctx->Map( g_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource ) != S_OK )
			return;
		if( ctx->Map( g_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource ) != S_OK )
			return;
		ImDrawVert* vtx_dst = ( ImDrawVert* )vtx_resource.pData;
		ImDrawIdx* idx_dst = ( ImDrawIdx* )idx_resource.pData;
		for( int n = 0; n < draw_data->CmdListsCount; n++ )
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			memcpy( vtx_dst, &cmd_list->VtxBuffer[0], cmd_list->VtxBuffer.size( ) * sizeof( ImDrawVert ) );
			memcpy( idx_dst, &cmd_list->IdxBuffer[0], cmd_list->IdxBuffer.size( ) * sizeof( ImDrawIdx ) );
			vtx_dst += cmd_list->VtxBuffer.size( );
			idx_dst += cmd_list->IdxBuffer.size( );
		}
		ctx->Unmap( g_pVB, 0 );
		ctx->Unmap( g_pIB, 0 );

		// Setup orthographic projection matrix into our constant buffer
		{
			D3D11_MAPPED_SUBRESOURCE mapped_resource;
			if( ctx->Map( g_pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource ) != S_OK )
				return;
			VERTEX_CONSTANT_BUFFER* constant_buffer = ( VERTEX_CONSTANT_BUFFER* )mapped_resource.pData;
			float L = 0.0f;
			float R = ImGui::GetIO( ).DisplaySize.x;
			float B = ImGui::GetIO( ).DisplaySize.y;
			float T = 0.0f;
			float mvp[4][4] =
			{
				{ 2.0f / ( R - L ),   0.0f,           0.0f,       0.0f },
				{ 0.0f,         2.0f / ( T - B ),     0.0f,       0.0f },
				{ 0.0f,         0.0f,           0.5f,       0.0f },
				{ ( R + L ) / ( L - R ),  ( T + B ) / ( B - T ),    0.5f,       1.0f },
			};
			memcpy( &constant_buffer->mvp, mvp, sizeof( mvp ) );
			ctx->Unmap( g_pVertexConstantBuffer, 0 );
		}

		// Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
		struct BACKUP_DX11_STATE
		{
			UINT                        ScissorRectsCount, ViewportsCount;
			D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			ID3D11RasterizerState*      RS;
			ID3D11BlendState*           BlendState;
			FLOAT                       BlendFactor[4];
			UINT                        SampleMask;
			UINT                        StencilRef;
			ID3D11DepthStencilState*    DepthStencilState;
			ID3D11ShaderResourceView*   PSShaderResource;
			ID3D11SamplerState*         PSSampler;
			ID3D11PixelShader*          PS;
			ID3D11VertexShader*         VS;
			UINT                        PSInstancesCount, VSInstancesCount;
			ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
			D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
			ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
			UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
			DXGI_FORMAT                 IndexBufferFormat;
			ID3D11InputLayout*          InputLayout;
		};
		BACKUP_DX11_STATE old;
		old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		ctx->RSGetScissorRects( &old.ScissorRectsCount, old.ScissorRects );
		ctx->RSGetViewports( &old.ViewportsCount, old.Viewports );
		ctx->RSGetState( &old.RS );
		ctx->OMGetBlendState( &old.BlendState, old.BlendFactor, &old.SampleMask );
		ctx->OMGetDepthStencilState( &old.DepthStencilState, &old.StencilRef );
		ctx->PSGetShaderResources( 0, 1, &old.PSShaderResource );
		ctx->PSGetSamplers( 0, 1, &old.PSSampler );
		old.PSInstancesCount = old.VSInstancesCount = 256;
		ctx->PSGetShader( &old.PS, old.PSInstances, &old.PSInstancesCount );
		ctx->VSGetShader( &old.VS, old.VSInstances, &old.VSInstancesCount );
		ctx->VSGetConstantBuffers( 0, 1, &old.VSConstantBuffer );
		ctx->IAGetPrimitiveTopology( &old.PrimitiveTopology );
		ctx->IAGetIndexBuffer( &old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset );
		ctx->IAGetVertexBuffers( 0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset );
		ctx->IAGetInputLayout( &old.InputLayout );

		// Setup viewport
		D3D11_VIEWPORT vp;
		memset( &vp, 0, sizeof( D3D11_VIEWPORT ) );
		vp.Width = ImGui::GetIO( ).DisplaySize.x;
		vp.Height = ImGui::GetIO( ).DisplaySize.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = vp.TopLeftY = 0.0f;
		ctx->RSSetViewports( 1, &vp );

		// Bind shader and vertex buffers
		unsigned int stride = sizeof( ImDrawVert );
		unsigned int offset = 0;
		ctx->IASetInputLayout( g_pInputLayout );
		ctx->IASetVertexBuffers( 0, 1, &g_pVB, &stride, &offset );
		ctx->IASetIndexBuffer( g_pIB, sizeof( ImDrawIdx ) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0 );
		ctx->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		ctx->VSSetShader( g_pVertexShader, NULL, 0 );
		ctx->VSSetConstantBuffers( 0, 1, &g_pVertexConstantBuffer );
		ctx->PSSetShader( g_pPixelShader, NULL, 0 );
		ctx->PSSetSamplers( 0, 1, &g_pFontSampler );

		// Setup render state
		const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
		ctx->OMSetBlendState( g_pBlendState, blend_factor, 0xffffffff );
		ctx->OMSetDepthStencilState( g_pDepthStencilState, 0 );
		ctx->RSSetState( g_pRasterizerState );

		// Render command lists
		int vtx_offset = 0;
		int idx_offset = 0;
		for( int n = 0; n < draw_data->CmdListsCount; n++ )
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size( ); cmd_i++ )
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if( pcmd->UserCallback )
				{
					pcmd->UserCallback( cmd_list, pcmd );
				}
				else
				{
					const D3D11_RECT r = { ( LONG )pcmd->ClipRect.x, ( LONG )pcmd->ClipRect.y, ( LONG )pcmd->ClipRect.z, ( LONG )pcmd->ClipRect.w };
					ctx->PSSetShaderResources( 0, 1, ( ID3D11ShaderResourceView** )&pcmd->TextureId );
					ctx->RSSetScissorRects( 1, &r );
					ctx->DrawIndexed( pcmd->ElemCount, idx_offset, vtx_offset );
				}
				idx_offset += pcmd->ElemCount;
			}
			vtx_offset += cmd_list->VtxBuffer.size( );
		}

		// Restore modified DX state
		ctx->RSSetScissorRects( old.ScissorRectsCount, old.ScissorRects );
		ctx->RSSetViewports( old.ViewportsCount, old.Viewports );
		ctx->RSSetState( old.RS ); if( old.RS ) old.RS->Release( );
		ctx->OMSetBlendState( old.BlendState, old.BlendFactor, old.SampleMask ); if( old.BlendState ) old.BlendState->Release( );
		ctx->OMSetDepthStencilState( old.DepthStencilState, old.StencilRef ); if( old.DepthStencilState ) old.DepthStencilState->Release( );
		ctx->PSSetShaderResources( 0, 1, &old.PSShaderResource ); if( old.PSShaderResource ) old.PSShaderResource->Release( );
		ctx->PSSetSamplers( 0, 1, &old.PSSampler ); if( old.PSSampler ) old.PSSampler->Release( );
		ctx->PSSetShader( old.PS, old.PSInstances, old.PSInstancesCount ); if( old.PS ) old.PS->Release( );
		for( UINT i = 0; i < old.PSInstancesCount; i++ ) if( old.PSInstances[i] ) old.PSInstances[i]->Release( );
		ctx->VSSetShader( old.VS, old.VSInstances, old.VSInstancesCount ); if( old.VS ) old.VS->Release( );
		ctx->VSSetConstantBuffers( 0, 1, &old.VSConstantBuffer ); if( old.VSConstantBuffer ) old.VSConstantBuffer->Release( );
		for( UINT i = 0; i < old.VSInstancesCount; i++ ) if( old.VSInstances[i] ) old.VSInstances[i]->Release( );
		ctx->IASetPrimitiveTopology( old.PrimitiveTopology );
		ctx->IASetIndexBuffer( old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset ); if( old.IndexBuffer ) old.IndexBuffer->Release( );
		ctx->IASetVertexBuffers( 0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset ); if( old.VertexBuffer ) old.VertexBuffer->Release( );
		ctx->IASetInputLayout( old.InputLayout ); if( old.InputLayout ) old.InputLayout->Release( );
	}
}

/**
* Registers a menu to appear in the top system menu bar
*/
void AXImGui::RegisterSystemDebugMenuItem( const AXString& path, SystemDebugMenuItem::CallbackFunction callbackFunction )
{
	std::vector< AXString > splitStrings;
	AXUtils::SplitString( path, '/', splitStrings );

	SystemDebugMenuItem::Collection* curCollection( &mSystemMenuItems );
	SystemDebugMenuItem* curItem( nullptr );

	for( const AXString& str : splitStrings )
	{ 
		if( curCollection )
		{
			auto it( curCollection->find( str ) );

			if( it != curCollection->end( ) )
			{
				if( it->second )
				{
					curItem = it->second;
					curCollection = &( curItem->mChildItems );
				}
			}
			else
			{
				curItem = new SystemDebugMenuItem( );

				curItem->mText = str;

				(*curCollection)[str] = curItem;
				curCollection = &( curItem->mChildItems );
			}
		}
	}

	if( curItem )
	{
		curItem->mCallbackFunction = callbackFunction;
	}
}

#if defined(AXPLATFORM_WINDOWS)

/**
* Windows function to handle input
*/
bool AXImGui::HandleWndProc(uint32_t umsg, uint64_t wparam, uint64_t lparam)
{
	if (ImGui::GetCurrentContext())
	{
		// Todo: Full of ImGui hacks
		ImGuiIO& io = ImGui::GetIO();

		switch (umsg)
		{
		case WM_LBUTTONDOWN:
			io.MouseDown[0] = true;
			return true;
		case WM_LBUTTONUP:
			io.MouseDown[0] = false;
			return true;
		case WM_RBUTTONDOWN:
			io.MouseDown[1] = true;
			return true;
		case WM_RBUTTONUP:
			io.MouseDown[1] = false;
			return true;
		case WM_MBUTTONDOWN:
			io.MouseDown[2] = true;
			return true;
		case WM_MBUTTONUP:
			io.MouseDown[2] = false;
			return true;
		case WM_MOUSEWHEEL:
			io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? +1.0f : -1.0f;
			return true;
		case WM_MOUSEMOVE:
			io.MousePos.x = (signed short)(lparam);
			io.MousePos.y = (signed short)(lparam >> 16);
			return true;
		case WM_KEYDOWN:
			if (wparam < 256)
				io.KeysDown[wparam] = 1;
			return true;
		case WM_KEYUP:
			if (wparam < 256)
				io.KeysDown[wparam] = 0;
			return true;
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			if (wparam > 0 && wparam < 0x10000)
				io.AddInputCharacter((unsigned short)wparam);
			return true;
		}
	}

	return false;
}

#endif // #if defined(AXPLATFORM_WINDOWS)

/**
* Renders out the system debug menu
*/
void AXImGui::RenderSystemDebugMenu( )
{
	if( ImGui::BeginMainMenuBar( ) )
	{
		RenderSystemDebugMenuRecursive( mSystemMenuItems );

		ImGui::EndMainMenuBar( );
	}
}


/**
* Renders out the system debug menu
*/
void AXImGui::RenderSystemDebugMenuRecursive( SystemDebugMenuItem::Collection& collection )
{
	for( auto it : collection )
	{
		if( it.second->mCallbackFunction )
		{
			it.second->mCallbackFunction( *(it.second) );
		}

		if( !it.second->mChildItems.empty( ) )
		{
			if( ImGui::BeginMenu( it.second->mText.c_str( ) ) )
			{
				RenderSystemDebugMenuRecursive( it.second->mChildItems );

				ImGui::EndMenu( );
			}
		}
	}
}

/**
* A callback function to draw the imgui demo window
*/
void AXImGui::ImGuiSystemDebugMenu_DemoWindowCallback( SystemDebugMenuItem& item )
{
	ImGui::MenuItem( item.mText.c_str( ), "", &mShouldRenderImGuiDemoWindow );
}

/**
* A callback function to draw the imgui style window
*/
void AXImGui::ImGuiSystemDebugMenu_StyleWindowCallback( SystemDebugMenuItem& item )
{
	ImGui::MenuItem( item.mText.c_str( ), "", &mShouldRenderImGuiStyleWindow );
}

/**
* A callback function to draw the imgui metrics window
*/
void AXImGui::ImGuiSystemDebugMenu_MetricsWindowCallback( SystemDebugMenuItem& item )
{
	ImGui::MenuItem( item.mText.c_str( ), "", &mShouldRenderImGuiMetricsWindow );
}

/**
* A callback function to draw the imgui user guide window
*/
void AXImGui::ImGuiSystemDebugMenu_UserGuideWindowCallback( SystemDebugMenuItem& item )
{
	ImGui::MenuItem( item.mText.c_str( ), "", &mShouldRenderImGuiUserGuideWindow );
}

void AXImGui::DoRenderPropertyToImGuiHelper( AXString& val )
{
	static char buffer[4098];
	strcpy_s( buffer, val.c_str( ) );

	ImGui::PushAllowKeyboardFocus( true );
	ImGui::InputText( "", buffer, 4098, ImGuiInputTextFlags_AutoSelectAll );
	ImGui::PopAllowKeyboardFocus( );

	val = buffer;
}

void AXImGui::DoRenderPropertyToImGuiHelper( int& val )
{
	ImGui::PushAllowKeyboardFocus( true );

	ImGui::InputInt( "", &val );

	ImGui::PopAllowKeyboardFocus( );
}

void AXImGui::DoRenderPropertyToImGuiHelper( uint8_t& val )
{
	ImGui::PushAllowKeyboardFocus( true );

	ImGui::InputInt( "", ( int* )( &val ) );

	ImGui::PopAllowKeyboardFocus( );
}

void AXImGui::DoRenderPropertyToImGuiHelper( uint32_t& val )
{
	ImGui::PushAllowKeyboardFocus( true );

	ImGui::InputInt( "", ( int* )( &val ) );

	ImGui::PopAllowKeyboardFocus( );
}

void AXImGui::DoRenderPropertyToImGuiHelper( bool& val )
{
	ImGui::Checkbox( "", &val );
}

void AXImGui::DoRenderPropertyToImGuiHelper( float& val )
{
	ImGui::DragFloat( "", &val );
}