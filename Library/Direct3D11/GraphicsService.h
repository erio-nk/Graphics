/*!
	@file	GraphicsService.h
	@author	E.Nakayam
*/

#include <memory>

#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>

#include "Misc/Handle.h"

typedef Handle<IDXGISwapChain> SwapChainHandle;
typedef Handle<ID3D11Device> DeviceHandle;
typedef Handle<ID3D11DeviceContext> DeviceContextHandle;


/*!
	@brief	グラフィックスサービスクラス。
*/
class GraphicsService
{
public:
	GraphicsService();
	~GraphicsService();
	
	bool Create(HWND hWnd);
	void Release();
	
	SwapChainHandle GetSwapChain();
	DeviceHandle GetDevice();
	DeviceContextHandle GetDeviceContext();
	
private:
	struct Impl;
	std::auto_ptr<Impl> _pimpl;
	
};	// end class GraphicsService

