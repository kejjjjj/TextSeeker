#pragma once

#ifndef inpf
#define inpf

#include "pch.hpp"

#define IDM_SEARCHBOX 2001

std::wstring UI_GetWindowText(HWND hWnd);

inline HWND g_hSearchBox;
inline HFONT SegoeUI;


LRESULT CALLBACK SearchBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateSearchBox(HWND parent);

#endif