/*!
	@file	DebugTrace.cpp
	@author	E.Nakayama
*/
#pragma once

#include <stdarg.h>

#include "DebugTrace.h"	

void DebugTrace(const TCHAR* format, ...)
{
	static TCHAR buf[1024];		// TODO: Thread Local Storage

#if defined(_WIN32)
	va_list va;
	va_start(va, format);
	_vstprintf_s(buf, format, va);
	va_end(va);
	OutputDebugString(buf);
	OutputDebugString(_T("\n"));
#else
	#error Not implemented
#endif
}
	