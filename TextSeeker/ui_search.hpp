
#ifndef uiserahc
#define uiserach

#include "pch.hpp"

class cSearchField : public cSeekerWindow
{
public:
	cSearchField(HWND hWnd_parent, HWND hwnd, HFONT _font) : hWnd(hwnd), parent(hWnd_parent), font(_font)
	{

	}

	static LRESULT CALLBACK ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT OnRenderSearchButton(LPDRAWITEMSTRUCT lpDrawItemStruct);

	HWND hWnd;

	
	HFONT font; 
	HWND h_gSearchButton;
	HWND h_gSearchField;
private:
	HWND parent;
	
};

void UI_CreateSearchField(HWND parent);

inline std::unique_ptr<cSearchField> searchField;

#endif