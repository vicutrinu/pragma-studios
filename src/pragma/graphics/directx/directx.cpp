#include "directx.h"
#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

namespace pragma
{
	namespace Impl
	{
		struct TData
		{
			ID3D10Device*	pD3DDevice;
			IDXGISwapChain* pSwapChain;
			ID3D10RenderTargetView* pRenderTargetView;
		};
	}

	using namespace Impl;

	static bool fatalError(const char* aMsg)
	{
		aMsg;
		return false;
	}

	static bool Init(Impl::TData& aData, HWND aWindow)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		//set buffer dimensions and format
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = 512;
		swapChainDesc.BufferDesc.Height = 512;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;;
		 
		//set refresh rate
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

		//sampling settings
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SampleDesc.Count = 1;
		 
		//output window handle
		swapChainDesc.OutputWindow = aWindow;
		swapChainDesc.Windowed = true;
		 
		//Create the D3D device
		//--------------------------------------------------------------
		 
		if ( FAILED( D3D10CreateDeviceAndSwapChain( NULL,
													D3D10_DRIVER_TYPE_HARDWARE,
													NULL,
													0,
													D3D10_SDK_VERSION,
													&swapChainDesc,
													&aData.pSwapChain,
													&aData.pD3DDevice ) ) ) return fatalError("D3D device creation failed");

		//Create render target view
		//--------------------------------------------------------------
		 
		//try to get the back buffer
		ID3D10Texture2D* pBackBuffer;
		 
		if ( FAILED( aData.pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer) ) ) return fatalError("Could not get back buffer");
		 
		//try to create render target view
		if ( FAILED( aData.pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &aData.pRenderTargetView) ) ) return fatalError("Could not create render target view");
		 
		//release the back buffer
		pBackBuffer->Release();
		 
		//set the render target
		aData.pD3DDevice->OMSetRenderTargets(1, &aData.pRenderTargetView, NULL);

		//create viewport structure
		D3D10_VIEWPORT viewPort;
		viewPort.Width = 800;
		viewPort.Height = 600;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
 
		//set the viewport
		aData.pD3DDevice->RSSetViewports(1, &viewPort);

		return true;
	}

	LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (msg) 
		{
			case WM_PAINT:
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			break;

			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}

	DirectXRenderer::DirectXRenderer()
	{
		HINSTANCE hInst = 0;
		mData = new(Impl::TData);
		if(mData)
		{
			WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MsgProc, 0L, 0L
							, hInst, LoadIcon(NULL, L"directX.ico"), LoadCursor(NULL,IDC_ARROW), NULL, NULL
							, L"DirectX10", NULL };
			RegisterClassEx( &wc );

			HWND hWnd = CreateWindow( L"DirectX10", L"DirectX10"
									, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT
									, 800, 600
									, NULL, NULL, NULL, NULL );

			ShowWindow( hWnd, SW_SHOWDEFAULT );
			Init(*(Impl::TData*)mData, hWnd);

			TData& lData = *(TData*)mData;
			float ClearColor[4] = { 0, 1, 1, 0};
			lData.pD3DDevice->ClearRenderTargetView(lData.pRenderTargetView, ClearColor);

			struct SimpleVertex
			{
				D3DXVECTOR3 Pos;  // Position
			};

			// Define the input layout
			D3D10_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
			};
			UINT numElements = sizeof(layout)/sizeof(layout[0]);

			// Create the input layout
			/*D3D10_PASS_DESC PassDesc;
			g_pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
			if( FAILED( g_pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, 
					PassDesc.IAInputSignatureSize, &g_pVertexLayout ) ) )
				return FALSE;*/
			// Set the input layout
			lData.pD3DDevice->IASetInputLayout( g_pVertexLayout );


			// Create vertex buffer
			SimpleVertex vertices[] =
			{
				D3DXVECTOR3( 0.0f, 0.5f, 0.5f ),
				D3DXVECTOR3( 0.5f, -0.5f, 0.5f ),
				D3DXVECTOR3( -0.5f, -0.5f, 0.5f ),
			};
			D3D10_BUFFER_DESC bd;
			bd.Usage = D3D10_USAGE_DEFAULT;
			bd.ByteWidth = sizeof( SimpleVertex ) * 3;
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			D3D10_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;
			if( FAILED( lData.pD3DDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer ) ) )
				return FALSE;

			// Set vertex buffer
			UINT stride = sizeof( SimpleVertex );
			UINT lData.pD3DDevice = 0;
			lData.pD3DDevice->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

			// Set lData.pD3DDevice topology
			lData.pD3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

			// Render a triangle
			D3D10_TECHNIQUE_DESC techDesc;
			g_pTechnique->GetDesc( &techDesc );
			for( UINT p = 0; p < techDesc.Passes; ++p )
			{
				g_pTechnique->GetPassByIndex( p )->Apply(0);
				lData.pD3DDevice->Draw( 3, 0 );
			}

			lData.pSwapChain->Present(0,0);
		}
	}

	DirectXRenderer::~DirectXRenderer()
	{
		delete[] mData;
		mData = 0;
	}
	
}
