#pragma once

#ifndef texseek_
#define texseek_

#include "pch.hpp"

#define IDM_BACKBUTTON 1001
#define IDM_FORWARDBUTTON 1002
#define IDM_SEARCHBOX 2001
#define IDM_TOPBAR 2003

#define IDM_FILEEXPLORER 2002


class cSeekerWindow
{
public:
	//virtual void OnResize(UINT width, UINT height) = 0;
	//virtual void OnPaint(WPARAM wParam, LPARAM lParam) = 0;
	//virtual void OnVerticalScroll(WPARAM wParam) = 0;

	RECT rect;

};

#endif