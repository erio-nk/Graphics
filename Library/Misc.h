/*!
	@file	Misc.h
	@author	E.Nakayama
*/

inline bool is_null(void* p)
{
	if (p == nullptr)
	{
		return true;
	}
	return false;
}

template <class T>
inline void SafeDelete(T*& p)
{
	if (is_null(p) == false)
	{
		delete p;
		p = NULL;
	}
}

template <class T>
inline void SafeRelease(T*& p)
{
	if (is_null(p) == false)
	{
		p->Release();
		p = NULL;
	}
}

#include "Misc/DebugTrace.h"
#include "Misc/FrameRateController.h"
#include "Misc/Handle.h"

