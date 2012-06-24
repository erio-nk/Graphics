/*!
	@file	Handle.h
	@author	E.Nakayama
*/
#pragma once
	
#include <memory>
	
template <class T>
class Handle
{
public:
	Handle()
	{
	}
	
	Handle(const std::shared_ptr<T> object)
	{
		_ptr = object;
	}
	
	Handle(const Handle& handle)
	{
		_ptr = handle._ptr;
	}
	
	
	T* Get()
	{
		if (std::shared_ptr<T> p = _ptr.lock())
		{
			return p.get();
		}
		return NULL;
	}
	
	const T* Get() const
	{
		if (const std::shared_ptr<T> p = _ptr.lock())
		{
			return p.get();
		}
		return NULL;
	}
	
private
	std::weak_ptr<T> _ptr;
};	// end class Handle
	