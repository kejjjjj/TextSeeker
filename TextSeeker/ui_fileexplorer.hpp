#ifndef fexlr
#define fexlr

#include "pch.hpp"

struct ScrollMetrics
{
	int horzScroll;
	int vertScroll;
	int upperWidth;

	int scrollX;
	int scrollY;

	int xMax;

};

#define EXPLORER_TOP_OFFSET 58

struct sExplorerButton
{
	sExplorerButton(HWND hwnd, const UINT _id, const std::wstring& click_path) : hWnd(hwnd), path(click_path), id(_id) {
		

	}
	~sExplorerButton()
	{
		DestroyWindow(hWnd);
	}
	HWND hWnd;
	std::wstring path;
	UINT id;
};

class cFileExplorer : public cSeekerWindow
{
public:
	cFileExplorer(HWND hWnd_parent, HWND hwnd) : hWnd(hwnd), parent(hWnd_parent)
	{

	}
	
	void OnPaint(WPARAM wParam, LPARAM lParam);
	void OnResize(uint32_t width, uint32_t height);
	void OnCreateScroll(uint32_t numLines);
	void OnHorizontalScroll(WPARAM wParam);

	static LRESULT CALLBACK ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND hWnd;
	ScrollMetrics sScroll{};
	std::list<sExplorerButton> buttons;
	std::wstring current_directory;
private:
	
	HWND parent;
};

inline std::unique_ptr<cFileExplorer> fileExplorer;

void DrawTextOnMainWindow(HWND hWnd);
void DrawTextOnWindow(HWND hWnd, const std::wstring& text, const RECT& where, UINT col = RGB(255, 255, 255), UINT fontSize = 20, bool vCenter = false, bool hCenter = false);
void UI_CreateFileExplorer(HWND parent);


#endif