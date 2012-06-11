#pragma once
	
class Application
{
public:
	Application();

	~Application();
	
	bool Create(HWND hWnd);

	void Release();
	
	void Update();
	
private:
	struct Impl;
	std::auto_ptr<Impl> _pimpl;
	
};	// end class Application
	