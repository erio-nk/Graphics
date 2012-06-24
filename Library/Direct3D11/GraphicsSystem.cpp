/*!
	@file	GraphicsSystem.cpp
	@author	E.Nakayama
*/

#include "GraphicsSystem.h"
	
struct GraphicsSystem::Impl
{
	std::shared_ptr<IDXGISwapChain> _pSwapChain;
	std::shared_ptr<ID3D11Device> _pDevice;
	std::shared_ptr<ID3D11DeviceContext> _pDeviceContext;
	
	Impl()
	{
	}
	
	~Impl()
	{
	}
};	// end struct GraphicsSystem::Impl

