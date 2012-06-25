/*!
	@file	GraphicsService.cpp
	@author	E.Nakayama
*/

#include <tchar.h>

#include "Misc.h"
#include "GraphicsService.h"

namespace
{

	typedef std::shared_ptr<IDXGISwapChain> SwapChainPtr;
	typedef std::shared_ptr<ID3D11Device> DevicePtr;
	typedef std::shared_ptr<ID3D11DeviceContext> DeviceContextPtr;

	inline void DebugTraceHresult(HRESULT hr)
	{
		DebugTrace(_T("%s : %s")
			, DXGetErrorString(hr)
			, DXGetErrorDescription(hr));
	}
}

struct GraphicsService::Impl
{
	SwapChainPtr _pSwapChain;
	DevicePtr _pDevice;
	DeviceContextPtr _pDeviceContext;
	
	Impl()
	{
	}
	
	~Impl()
	{
		Release();
	}

	void Release()
	{
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
		
		IDXGISwapChain* pSwapChainTmp;
		ID3D11Device* pDeviceTmp;
		ID3D11DeviceContext* pDeviceContextTmp;
		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL	// pAdapter
			, D3D_DRIVER_TYPE_HARDWARE	// DriverType
			, NULL	// Software
			, 0	// Flags
			, NULL	// pFeatureLevels
			, 0// FeatureLevels
			, D3D11_SDK_VERSION
			, &sd
			, &pSwapChainTmp
			, &pDeviceTmp
			, &featureLevel
			, &pDeviceContextTmp
			);
		if (FAILED(hr))
		{
			DebugTraceHresult(hr);
			return false;
		}
		_pSwapChain.reset(pSwapChainTmp, &SafeRelease<SwapChainPtr::element_type>);
		_pDevice.reset(pDeviceTmp, &SafeRelease<DevicePtr::element_type>);
		_pDeviceContext.reset(pDeviceContextTmp, &SafeRelease<DeviceContextPtr::element_type>);

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
		
		return true;
	}
	
	SwapChainHandle GetSwapChain()
	{
		return _pSwapChain;
	}
	
	DeviceHandle GetDevice()
	{
		return _pDevice;
	}

	DeviceContextHandle GetDeviceContext()
	{
		return _pDeviceContext;
	}
	
};	// end struct GraphicsService::Impl


GraphicsService::GraphicsService()
	: _pimpl(new Impl())
{
}

GraphicsService::~GraphicsService()
{
}

bool GraphicsService::Create(HWND hWnd)
{
	return _pimpl->Create(hWnd);
}

void GraphicsService::Release()
{
	_pimpl->Release();
}

SwapChainHandle GraphicsService::GetSwapChain()
{
	return _pimpl->GetSwapChain();
}

DeviceHandle GraphicsService::GetDevice()
{
	return _pimpl->GetDevice();
}

DeviceContextHandle GraphicsService::GetDeviceContext()
{
	return _pimpl->GetDeviceContext();
}

