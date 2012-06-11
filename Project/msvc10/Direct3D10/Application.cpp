#include "Application.h"

#include "Misc/FrameRateController.h"

struct Application::Impl
{
	FrameRateController _frameRateController;

	bool Create(HWND hWnd)
	{
		return true;
	}
	
	void Release()
	{
	}
	
	void Update()
	{
		_frameRateController.ChangeFrame();
		DebugTrace(_T("%.2f"), _frameRateController.GetCurrentFPS());
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
