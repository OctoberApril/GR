#pragma once
#include <winnt.h>
#include <winerror.h>
#include <exception>

static void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr)) throw std::exception("HRESULT IS NOT SUCCESS");
}