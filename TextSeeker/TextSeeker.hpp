#pragma once

#ifndef texseek_
#define texseek_

#include "pch.hpp"

class cSeekerWindow
{
public:
	virtual void OnResize(UINT width, UINT height) = 0;
	virtual void OnPaint(WPARAM wParam, LPARAM lParam) = 0;
	static LRESULT CALLBACK ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	RECT rect;

};

#endif