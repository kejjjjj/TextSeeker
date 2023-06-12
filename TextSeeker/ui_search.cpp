#include "pch.hpp"


void UI_CreateSearchField(HWND parent)
{
	RECT re;
	GetClientRect(parent, &re);

	int free_space = re.right - fileExplorer->rect.right - 16;
	int height = re.bottom - re.top;


	HWND hwnd = CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
		fileExplorer->rect.right + 16, fileExplorer->rect.top, free_space - 16 + 32, height/2, parent, (HMENU)0, GetModuleHandle(NULL), NULL);

    if (!IsWindow(hwnd)) {
        throw (std::format(L"exception caught while creating window:\n{}", get_last_error()));
        return;
    }

    HFONT SegoeUI = CreateFont(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    searchField = std::unique_ptr<cSearchField>(new cSearchField(parent, hwnd, SegoeUI));

    searchField->rect = { fileExplorer->rect.right + 16, fileExplorer->rect.top, free_space - 16 + 32, height/2 };
 

    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(hwnd, GWLP_WNDPROC));
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)searchField->ProcHandler);

    SendMessage(hwnd, WM_CREATE, NULL, NULL);

	SetFocus(hwnd);
	EnableWindow(hwnd, TRUE);
	SetCaretPos(0, 0);

}

LRESULT CALLBACK cSearchField::ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::wstring str;
    HWND hwnd;
    switch (message)
    {
    case WM_CREATE:

        searchField->h_gSearchButton = CreateWindowExW(0, L"BUTTON", NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            searchField->rect.left + (searchField->rect.right - searchField->rect.left)/2, searchField->rect.bottom,
            30, 30, searchField->hWnd, (HMENU)IDM_SEARCHBUTTON, GetModuleHandle(NULL), NULL);

        searchField->h_gSearchField = CreateWindowExW(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE,
            0, 0, searchField->rect.right, searchField->rect.bottom - 32 - 16, searchField->hWnd, (HMENU)IDM_SEARCHFIELD, GetModuleHandle(NULL), NULL);

        hwnd = searchField->h_gSearchButton;

        SendMessage(hwnd, WM_SETFONT, (WPARAM)searchField->font, TRUE);
        //SendMessage(hwnd, EM_LIMITTEXT, MAX_PATH, TRUE);
        SetWindowText(hwnd, L"Search...");

        return 0;

    case WM_PAINT:
        return 0;
        //case WM_COMMAND:

        //    if (LOWORD(wParam) == IDM_SEARCHBOX && HIWORD(wParam) == EN_SETFOCUS) {
        //        SendMessage(fileNavigation->g_hSearchBar, EN_SETFOCUS, 0, -1);
        //        

        //    }

        break;
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;

        if (lpDrawItemStruct->CtlType == ODT_BUTTON)
        {
            if (wParam == IDM_SEARCHBUTTON)
                return searchField->OnRenderSearchButton(lpDrawItemStruct);

            //else if (wParam == IDM_FORWARDBUTTON)
            //    return searchField->OnRenderRedoButton(lpDrawItemStruct);
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, RGB(25, 25, 25));
        SetTextColor(hdc, RGB(255, 255, 255)); // White text color

        // Return the handle to a brush for the background color
        HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 25));
        return (LRESULT)hBrush;


    }
    case WM_DESTROY:
        DeleteObject(searchField->font);
        break;

    default:
        break;
    }

    return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA), hWnd, message, wParam, lParam);

}

LRESULT cSearchField::OnRenderSearchButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    HDC hdcButton = lpDrawItemStruct->hDC;
    RECT rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;

    SetBkMode(hdcButton, TRANSPARENT);

    HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 25));

    HICON icon = LoadIconW(NULL, IDI_HAND);

    FillRect(hdcButton, &rect, hBrush);

    DrawIcon(hdcButton, rect.left, rect.top, icon);

    DestroyIcon(icon);
    DeleteObject(hBrush);

    return TRUE;
}
