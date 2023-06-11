
#ifndef uiserahc
#define uiserach

#include "pch.hpp"

class cSearchField : public cSeekerWindow
{
public:
	cSearchField(HWND hWnd_parent, HWND hwnd, HFONT _font) : hWnd(hwnd), parent(hWnd_parent), font(_font)
	{

	}

	HWND hWnd;
private:
	HWND parent;
	HFONT font;
};

void UI_CreateSearchField(HWND parent);

inline std::unique_ptr<cSearchField> searchField;

#endif