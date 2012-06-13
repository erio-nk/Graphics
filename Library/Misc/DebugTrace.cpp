/*!
	@file	DebugTrace.cpp
	@author	E.Nakayama
*/
#pragma once

#include <stdarg.h>

#include "DebugTrace.h"	

void DebugTraceA(const char* format, ...)
{
	static char buf[1024];		// TODO: Thread Local Storage

#if defined(_WIN32)
	va_list va;
	va_start(va, format);
	vsprintf_s(buf, format, va);
	va_end(va);
	OutputDebugStringA(buf);
	OutputDebugStringA("\n");
#else
	#error Not implemented
#endif
}

void DebugTraceW(const wchar_t* format, ...)
{
	static wchar_t buf[1024];		// TODO: Thread Local Storage

#if defined(_WIN32)
	va_list va;
	va_start(va, format);
	vswprintf_s(buf, format, va);
	va_end(va);
	OutputDebugStringW(buf);
	OutputDebugStringW(L"\n");
#else
	#error Not implemented
#endif
}
	