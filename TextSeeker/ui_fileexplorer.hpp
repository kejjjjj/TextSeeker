#ifndef fexlr
#define fexlr

#include "pch.hpp"
void DrawTextOnMainWindow(HWND hWnd);
void UI_CreateFileExplorer(HWND parent);
LRESULT CALLBACK FileExplorerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


void UI_ResizeFileExplorer(uint32_t width, uint32_t height);

inline HWND g_hFileExplorer;


#endif