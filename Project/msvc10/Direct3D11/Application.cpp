#include "Application.h"
	
#include <boost/tokenizer.hpp>

#pragma comment( lib, "dxgi.lib" )

namespace
{
	inline void ShaderCompileError(ID3D10Blob* pErrorsBlob)
	{
		if (is_null(pErrorsBlob))
		{
			return;
		}

		void* pError = pErrorsBlob->GetBufferPointer();

		typedef boost::char_separator<char> ErrorMessageSeparator;
		typedef boost::tokenizer<ErrorMessageSeparator> ErrorMessages;

		ErrorMessageSeparator sep("\n");
		std::string allMessages(reinterpret_cast<char*>(pError)); 
		ErrorMessages messages(allMessages, sep);
		ErrorMessages::iterator itr = messages.begin();
		ErrorMessages::iterator itrEnd = messages.end();
		for (; itr != itrEnd; ++itr)
		{
			DebugTraceA(itr->c_str());
		}
	}

	inline void DebugTraceHresult(HRESULT hr)
	{
		DebugTrace(_T("%s : %s")
			, DXGetErrorString(hr)
			, DXGetErrorDescription(hr));
	}
}

struct Application::Impl
{
	FrameRateController _frameRateController;
	
	IDXGISwapChain* _pSwapChain;
	ID3D11Device* _pDevice;
	ID3D11DeviceContext* _pDeviceContext;
	
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11Texture2D* _pDepthStencilBuffer;
	ID3D11DepthStencilView* _pDepthStencilView;

	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;

	ID3D11Buffer* _pVertexBuffer;
	ID3D11InputLayout* _pInputLayout;
	
	ID3D11Buffer* _pConstantBuffer;
	struct ConstantBuffer
	{
		XMFLOAT4X4 World;
	};
	
//	XMMATRIX _projection;	// x86ビルドでヒープ上に確保するとアライメントが正しく行われない。
	XMFLOAT4X4 _projection;
	
	Impl()
		: _pSwapChain(NULL)
		, _pDevice(NULL)
		, _pDeviceContext(NULL)
		, _pRenderTargetView(NULL)
		, _pDepthStencilBuffer(NULL)
		, _pDepthStencilView(NULL)
		, _pVertexShader(NULL)
		, _pPixelShader(NULL)
		, _pVertexBuffer(NULL)
		, _pInputLayout(NULL)
		, _pConstantBuffer(NULL)
	{
	}

	void Release()
	{
		SafeRelease(_pConstantBuffer);

		SafeRelease(_pInputLayout);
		SafeRelease(_pVertexBuffer);
		
		SafeRelease(_pVertexShader);
		SafeRelease(_pPixelShader);
		
		SafeRelease(_pDepthStencilView);
		SafeRelease(_pDepthStencilBuffer);
		SafeRelease(_pRenderTargetView);

		SafeRelease(_pSwapChain);
		SafeRelease(_pDevice);
		SafeRelease(_pDeviceContext);
	}

	bool Create(HWND hWnd)
	{
		// デバイスとスワップチェーンの作成。
		RECT rectClient;
		GetClientRect(hWnd, &rectClient);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = rectClient.right - rectClient.left;
		sd.BufferDesc.Height = rectClient.bottom - rectClient.top;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;//1;
		sd.Windowed = TRUE;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			
		D3D_FEATURE_LEVEL featureLevel;
		
		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL	// pAdapter
			, D3D_DRIVER_TYPE_HARDWARE	// DriverType
			, NULL	// Software
			, 0	// Flags
			, NULL	// pFeatureLevels
			, 0// FeatureLevels
			, D3D11_SDK_VERSION
			, &sd
			, &_pSwapChain
			, &_pDevice
			, &featureLevel
			, &_pDeviceContext
			);
		if (FAILED(hr))
		{
			DebugTraceHresult(hr);
			return false;
		}

		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_0:	DebugTrace(_T("D3D_FEATURE_LEVEL_11_0")); break;
		case D3D_FEATURE_LEVEL_10_0:	DebugTrace(_T("D3D_FEATURE_LEVEL_10_0")); break;
		case D3D_FEATURE_LEVEL_10_1:	DebugTrace(_T("D3D_FEATURE_LEVEL_10_1")); break;
		case D3D_FEATURE_LEVEL_9_3:		DebugTrace(_T("D3D_FEATURE_LEVEL_9_3")); break;
		case D3D_FEATURE_LEVEL_9_2:		DebugTrace(_T("D3D_FEATURE_LEVEL_9_2")); break;
		case D3D_FEATURE_LEVEL_9_1:		DebugTrace(_T("D3D_FEATURE_LEVEL_9_1")); break;
		default:						DebugTrace(_T("Unknown")); break;
		}
		
		// バックバッファの取得とレンダーターゲットビューの作成。
		ID3D11Texture2D* pBackBuffer;
		hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr))
		{
			return false;
		}
		
		hr = _pDevice->CreateRenderTargetView(pBackBuffer, NULL, &_pRenderTargetView);
		if (FAILED(hr))
		{
			return false;
		}
		
		// 深度ステンシルバッファとビューの作成。
		D3D11_TEXTURE2D_DESC dd;
		ZeroMemory(&dd, sizeof(dd));
		dd.Width = sd.BufferDesc.Width;
		dd.Height = sd.BufferDesc.Height;
		dd.MipLevels = 1;
		dd.ArraySize = 1;
		dd.Format = DXGI_FORMAT_D32_FLOAT;
		dd.SampleDesc.Count = 1;
		dd.SampleDesc.Quality = 0;
		dd.Usage = D3D11_USAGE_DEFAULT;
		dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dd.CPUAccessFlags = 0;
		dd.MiscFlags = 0;
		hr = _pDevice->CreateTexture2D(&dd, NULL, &_pDepthStencilBuffer);
		if (FAILED(hr))
		{
			return false;
		}
		
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		dsvd.Format = DXGI_FORMAT_D32_FLOAT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvd.Texture2D.MipSlice = 0;
		_pDevice->CreateDepthStencilView(_pDepthStencilBuffer, &dsvd, &_pDepthStencilView);
		
		// レンダーターゲットを設定。
		_pDeviceContext->OMSetRenderTargets(1, &_pRenderTargetView, _pDepthStencilView);
		
		// ビューポートの設定。
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<float>(sd.BufferDesc.Width);
		vp.Height = static_cast<float>(sd.BufferDesc.Height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		_pDeviceContext->RSSetViewports(1, &vp);
		
		
		// 頂点シェーダの作成。
		ID3D10Blob* pErrorsBlob = NULL;
		ID3D10Blob* pVertexShaderBytecode = NULL;
		hr = D3DX11CompileFromFile(_T("D:\\works\\git\\Graphics\\Project\\msvc10\\Direct3D11\\simple.sh")
			, NULL
			, NULL
			, "VS"
			, "vs_4_0"
			, D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR
			, 0
			, NULL
			, &pVertexShaderBytecode
			, &pErrorsBlob
			, NULL
			);
		if (FAILED(hr))
		{
			ShaderCompileError(pErrorsBlob);
			SafeRelease(pErrorsBlob);
			return false;
		}
			
		hr = _pDevice->CreateVertexShader(pVertexShaderBytecode->GetBufferPointer()
			, pVertexShaderBytecode->GetBufferSize()
			, NULL
			, &_pVertexShader
			);
		if (FAILED(hr))
		{
			SafeRelease(pVertexShaderBytecode);
			return false;
		}
		_pDeviceContext->VSSetShader(_pVertexShader, NULL, 0);

		// ピクセルシェーダの作成。
		ID3D10Blob* pPixelShaderBytecode = NULL;
		hr = D3DX11CompileFromFile(_T("D:\\works\\git\\Graphics\\Project\\msvc10\\Direct3D11\\simple.sh")
			, NULL
			, NULL
			, "PS"
			, "ps_4_0"
			, D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR
			, 0
			, NULL
			, &pPixelShaderBytecode
			, &pErrorsBlob
			, NULL
			);
		if (FAILED(hr))
		{
			ShaderCompileError(pErrorsBlob);
			SafeRelease(pErrorsBlob);
			return false;
		}
			
		hr = _pDevice->CreatePixelShader(pPixelShaderBytecode->GetBufferPointer()
			, pPixelShaderBytecode->GetBufferSize()
			, NULL
			, &_pPixelShader
			);
		SafeRelease(pPixelShaderBytecode);
		if (FAILED(hr))
		{
			SafeRelease(pVertexShaderBytecode);
			return false;
		}
		_pDeviceContext->PSSetShader(_pPixelShader, NULL, 0);

		// 頂点バッファの生成。
		XMFLOAT3 vertices[] =
		{
			XMFLOAT3(0.0f, 0.0f, 0.f)
			, XMFLOAT3(-1.0f, 0.0f, 0.f)
			, XMFLOAT3(0.0f, 1.0f, 0.f)
		};

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(vertices);
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		
		D3D11_SUBRESOURCE_DATA sub;
		ZeroMemory(&sub, sizeof(sub));
		sub.pSysMem = vertices;
		
		hr = _pDevice->CreateBuffer(&bd, &sub, &_pVertexBuffer);
		if (FAILED(hr))
		{
			return false;
		}
		
		// 入力レイアウトの生成。
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0/*InputSlot*/, 0/*AlignedByteOffset*/, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		hr = _pDevice->CreateInputLayout(
			layout
			, _countof(layout)
			, pVertexShaderBytecode->GetBufferPointer()
			, pVertexShaderBytecode->GetBufferSize()
			, &_pInputLayout
			);

		SafeRelease(pVertexShaderBytecode);
		
		// 定数バッファの生成。
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		hr = _pDevice->CreateBuffer(&bd, NULL, &_pConstantBuffer);
		if (FAILED(hr))
		{
			return false;
		}
		
		XMMATRIX tmp = XMMatrixPerspectiveFovLH(
//		_projection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(30.0f)
			, static_cast<float>(sd.BufferDesc.Width) / static_cast<float>(sd.BufferDesc.Height)
			, 1.0f	// nearZ
			, 20.0f	// farZ
			);
		XMStoreFloat4x4(&_projection, tmp);

		return true;
	}
	
	
	void Update()
	{
		float clearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		_pDeviceContext->ClearRenderTargetView(_pRenderTargetView, clearColor);
		_pDeviceContext->ClearDepthStencilView(_pDepthStencilView
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		const UINT NUM_BUFFERS = 1;
		ID3D11Buffer* vertexBuffers[] = { _pVertexBuffer };
		UINT strides[] = { sizeof(XMFLOAT3) };
		UINT offsets[] = { 0 };
		_pDeviceContext->IASetVertexBuffers(0, NUM_BUFFERS, vertexBuffers, strides, offsets);
		_pDeviceContext->IASetInputLayout(_pInputLayout);
		_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		XMVECTORF32 eye= { 0.0f, 5.0f, -5.0f, 1.0f };
		XMVECTORF32 at = { 0.0f, 0.0f,  0.0f, 1.0f };
		XMVECTORF32 up = { 0.0f, 1.0f,  0.0f, 1.0f };
		XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
		
		XMMATRIX world = view *  XMLoadFloat4x4(&_projection);;
		world = XMMatrixTranspose(world);
		
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = _pDeviceContext->Map(
			_pConstantBuffer
			, 0
			, D3D11_MAP_WRITE_DISCARD
			, 0
			, &mapped);
		if (SUCCEEDED(hr))
		{
			memcpy(mapped.pData, &world, sizeof(XMMATRIX));
			_pDeviceContext->Unmap(_pConstantBuffer, 0);
		}

		ID3D11Buffer* constantBuffers[] = { _pConstantBuffer };
		_pDeviceContext->VSSetConstantBuffers(0, 1, constantBuffers);
		_pDeviceContext->PSSetConstantBuffers(0, 1, constantBuffers);
		
		_pDeviceContext->Draw(3, 0);
		
		_pSwapChain->Present(NULL, NULL);
		
		_frameRateController.ChangeFrame();
//		DebugTrace(_T("%.2f"), _frameRateController.GetCurrentFPS());
	}
	
};	// end struct Application::Impl

Application::Application()
	: _pimpl(new Impl())
{
}

Application::~Application()
{
}

bool Application::Create(HWND hWnd)
{
	return _pimpl->Create(hWnd);
}

void Application::Release()
{
	_pimpl->Release();
}

void Application::Update()
{
	_pimpl->Update();
}
