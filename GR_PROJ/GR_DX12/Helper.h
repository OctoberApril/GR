#pragma once
#include <winnt.h>
#include <winerror.h>
#include <exception>

static void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr)) throw std::exception("HRESULT IS NOT SUCCESS");
}

static size_t AlignUp(size_t size,int alignment)
{
	return (size + alignment) & ~(alignment - 1);
}

static size_t AlignDown(size_t size,int alignment)
{
	return size & ~(alignment - 1);
}