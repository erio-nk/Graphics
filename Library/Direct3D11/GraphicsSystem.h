/*!
	@file	GraphicsSystem.h
	@author	E.Nakayam
*/

#include <memory>

#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>

class GraphicsSystem
{
public:
	GraphicsSystem();
	~GraphicsSystem();
	
private:
	struct Impl;
	std::auto_ptr<Impl> _pimpl;
	
};	// end class GraphicsSystem

