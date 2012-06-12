#include "Application.h"

struct Application::Impl
{
	FrameRateController _frameRateController;
	
	IDXGISwapChain* _pSwapChain;
	ID3D11Device* _pDevice;
	ID3D11DeviceContext* _pDeviceContext;
	
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11Texture2D* _pDepthStencilBuffer;
	ID3D11DepthStencilView* _pDepthStencilView;

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
		
		return true;
	}
	
	void Release()
	{
		SafeRelease(_pDepthStencilView);
		SafeRelease(_pDepthStencilBuffer);
		SafeRelease(_pRenderTargetView);

		SafeRelease(_pSwapChain);
		SafeRelease(_pDevice);
		SafeRelease(_pDeviceContext);
	}
	
	void Update()
	{
		float clearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		_pDeviceContext->ClearRenderTargetView(_pRenderTargetView, clearColor);
		_pDeviceContext->ClearDepthStencilView(_pDepthStencilView
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
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
