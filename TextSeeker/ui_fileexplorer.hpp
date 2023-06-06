#ifndef fexlr
#define fexlr

#include "pch.hpp"

class cFileExplorer : public cSeekerWindow
{
public:
	cFileExplorer(HWND hWnd_parent, HWND hwnd) : hWnd(hwnd), parent(hWnd_parent)
	{

	}

	void OnPaint(WPARAM wParam, LPARAM lParam);
	void OnResize(uint32_t width, uint32_t height);
	static LRESULT CALLBACK ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND hWnd;
private:
	
	HWND parent;
};

inline std::unique_ptr<cFileExplorer> fileExplorer;

void DrawTextOnMainWindow(HWND hWnd);
void UI_CreateFileExplorer(HWND parent);


inline HWND g_hFileExplorer;


#endif