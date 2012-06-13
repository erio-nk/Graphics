/*!
	@file	DebugTrace.h
	@author	E.Nakayama
*/
#pragma once

void DebugTraceA(const char* format, ...);

void DebugTraceW(const wchar_t* format, ...);

#if defined(_UNICODE)
	#define DebugTrace DebugTraceW
#else
	#define DebugTrace DebugTraceA
#endif
	