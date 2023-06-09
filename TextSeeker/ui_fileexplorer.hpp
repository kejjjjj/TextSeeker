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
	int nPage;

};

#define EXPLORER_TOP_OFFSET 58
#define IDM_DIRECTORY_ITEMS 2005ull


struct sExplorerButton
{
	sExplorerButton(HWND hwnd, const UINT _id, const sFile& _file, const RECT& bounds, HFONT _font) 
		: hWnd(hwnd), id(_id), file(_file), rect(bounds), font(_font) {
		

	}
	~sExplorerButton() = default;

	void Destroy()
	{
		DestroyWindow(hWnd);
		DeleteObject(font);
	}

	sFile file;
	HWND hWnd;
	UINT id;
	RECT rect;
	HFONT font;
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
	void OnVerticalScroll(WPARAM wParam);
	void OnWheelScroll(WPARAM wParam);
	sExplorerButton* FindButtonByHWND(HWND hWnd);
	sExplorerButton* GetHoveredButton(POINT mouse);

	void OnPopulateButtons(const std::wstring& loc);

	LRESULT OnRenderDirectoryButton(LPDRAWITEMSTRUCT lpDrawItemStruct);

	static LRESULT CALLBACK ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK OwnerDrawButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam,
		LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	HWND hWnd;
	ScrollMetrics sScroll{};
	std::list<sExplorerButton> buttons;
	std::wstring current_directory;

	std::list<std::string> undoHistory;
	std::list<std::string> redoHistory;
private:
	
	HWND parent;
};

inline std::unique_ptr<cFileExplorer> fileExplorer;

void DrawTextOnWindow(HWND hWnd, const std::wstring& text, const RECT& where, UINT col = RGB(255, 255, 255), UINT fontSize = 20, bool vCenter = false, bool hCenter = false);
void UI_CreateFileExplorer(HWND parent);


#endif