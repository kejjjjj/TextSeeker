#pragma once

#ifndef inpf
#define inpf

#include "pch.hpp"



std::wstring UI_GetWindowText(HWND hWnd);
bool isHovered(const RECT& r, const LPPOINT p);

#define BACKBUTTON_X 6
#define FORWARDBUTTON_X 40
#define SEARCHBOX_X (FORWARDBUTTON_X + 30 + 15)

class cFileNavigation : public cSeekerWindow
{
public:
	cFileNavigation(HWND hWnd_parent, HWND hwnd, HFONT _font) : hWnd(hwnd), parent(hWnd_parent), font(_font)
	{
		
	}

	void OnPaint(WPARAM wParam, LPARAM lParam);
	void OnResize(uint32_t width, uint32_t height);
	LRESULT OnRenderUndoButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
	LRESULT OnRenderRedoButton(LPDRAWITEMSTRUCT lpDrawItemStruct);

	static LRESULT CALLBACK ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SearchBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND hWnd;
	HWND g_hBackButton;
	HWND g_hForwardButton;
	HWND g_hSearchBar;

	HFONT font;
private:

	HWND parent;
};


void UI_CreateSearchBox(HWND parent);

inline std::unique_ptr<cFileNavigation> fileNavigation;


#endif