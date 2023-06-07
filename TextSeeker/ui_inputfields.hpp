#pragma once

#ifndef inpf
#define inpf

#include "pch.hpp"

#define IDM_SEARCHBOX 2001
#define IDM_FILEEXPLORER 2002

std::wstring UI_GetWindowText(HWND hWnd);
bool isHovered(const RECT& r, const LPPOINT p);

inline HWND g_hSearchBox;
inline HFONT SegoeUI;


LRESULT CALLBACK SearchBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateSearchBox(HWND parent);

#endif