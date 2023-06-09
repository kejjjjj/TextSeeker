#include "pch.hpp"

std::wstring UI_GetWindowText(HWND hWnd)
{
    if (!hWnd) {
        MessageBox(NULL, L"called UI_GetWindowText with a NULL hWnd", L"Error!", MB_ICONERROR);
        return L"";
    }
    int textLength = GetWindowTextLength(hWnd);

    wchar_t* textBuffer = new wchar_t[textLength + 1];

    GetWindowText(hWnd, textBuffer, textLength + 1);

    std::wstring buf = textBuffer;
    delete[] textBuffer;

    return buf;
}
bool isHovered(const RECT& r, const LPPOINT p)
{
    return 
        p->x > r.left && p->x < r.right &&
        p->y > r.top && p->y < r.bottom;
}

void UI_CreateSearchBox(HWND parent)
{
    RECT r;
    GetClientRect(parent, &r);

    auto width = r.right - r.left;

    HWND hwnd = CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 23, 300, 30, parent, (HMENU)IDM_TOPBAR, GetModuleHandle(NULL), NULL);


    if (!IsWindow(hwnd)) {
        throw (std::format(L"exception caught while creating window:\n{}", get_last_error()));
        return;
    }

    HFONT SegoeUI = CreateFont(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    fileNavigation = std::unique_ptr<cFileNavigation>(new cFileNavigation(parent, hwnd, SegoeUI));




    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(hwnd, GWLP_WNDPROC));
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)fileNavigation->ProcHandler);

    SendMessage(hwnd, WM_CREATE, NULL, NULL);

}

LRESULT CALLBACK cFileNavigation::ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::wstring str;
    HWND hwnd;
    switch (message)
    {
    case WM_CREATE:
        fileNavigation->g_hBackButton = CreateWindowExW(0, L"BUTTON", L"BACK", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            6, 0, 30, 30, fileNavigation->hWnd, (HMENU)IDM_BACKBUTTON, GetModuleHandle(NULL), NULL);

        fileNavigation->g_hForwardButton = CreateWindowExW(0, L"BUTTON", L"FORWARD", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            40, 0, 30, 30, fileNavigation->hWnd, (HMENU)IDM_FORWARDBUTTON, GetModuleHandle(NULL), NULL);

        fileNavigation->g_hSearchBar = CreateWindowExW(0, L"EDIT", L"aaa", WS_CHILD | WS_VISIBLE | WS_BORDER,
            40*2+15, 0, 300, 30, fileNavigation->hWnd, (HMENU)IDM_SEARCHBOX, GetModuleHandle(NULL), NULL);

        hwnd = fileNavigation->g_hSearchBar;

        SendMessage(hwnd, WM_SETFONT, (WPARAM)fileNavigation->font, TRUE);
       // SendMessage(hwnd, EM_SETRECT, TRUE, (LPARAM)&rect);
        SendMessage(hwnd, EM_LIMITTEXT, MAX_PATH, TRUE);
        SetWindowText(hwnd, L"Search...");

        if (!IsWindow(fileNavigation->g_hBackButton) || !IsWindow(fileNavigation->g_hSearchBar)) {
            throw (std::format(L"exception caught while creating window:\n{}", get_last_error()));
            return 0;
        }

        return 0;

    case WM_SETFOCUS:
        SetWindowText(hWnd, L"");
        break;
    case WM_KILLFOCUS:
        if (GetWindowTextLength(hWnd) == 0) {
            SetWindowText(hWnd, L"Search...");

        }
        break;

    case WM_KEYDOWN:

        if (wParam == (WPARAM)VK_RETURN) {
            str = UI_GetWindowText(hWnd);
        }
        break;
    case WM_PAINT:
        return 0;
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;

        if (lpDrawItemStruct->CtlType == ODT_BUTTON)
        {
            if (wParam == IDM_BACKBUTTON)
                return fileNavigation->OnRenderUndoButton(lpDrawItemStruct);
            else if (wParam == IDM_FORWARDBUTTON)
                return fileNavigation->OnRenderRedoButton(lpDrawItemStruct);
        }
        break;
    }
    case WM_DESTROY:
        DeleteObject(fileNavigation->font);
        break;

    default:
        break;
    }

    return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA), hWnd, message, wParam, lParam);

}

LRESULT cFileNavigation::OnRenderUndoButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    RECT r;
    GetClientRect(parent, &r);

    r.top = EXPLORER_TOP_OFFSET;
    r.bottom = r.top + 30;

    r.left = 6;
    r.right = r.left + 30;

    HDC hdcButton = lpDrawItemStruct->hDC;
    RECT rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;

    SetBkMode(hdcButton, TRANSPARENT);

    HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 25));

    HICON icon = LoadIconW(NULL, IDI_HAND);

    FillRect(hdcButton, &r, hBrush);

    DrawIcon(hdcButton, rect.left, rect.top, icon);

    DestroyIcon(icon);
    DeleteObject(hBrush);

    return TRUE;
}
LRESULT cFileNavigation::OnRenderRedoButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    RECT r;
    GetClientRect(parent, &r);

    r.top = EXPLORER_TOP_OFFSET;
    r.bottom = r.top + 30;

    r.left = 6;
    r.right = r.left + 30;

    HDC hdcButton = lpDrawItemStruct->hDC;
    RECT rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;

    SetBkMode(hdcButton, TRANSPARENT);

    HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 25));

    HICON icon = LoadIconW(NULL, IDI_HAND);

    FillRect(hdcButton, &r, hBrush);

    DrawIcon(hdcButton, rect.left, rect.top, icon);

    DestroyIcon(icon);
    DeleteObject(hBrush);

    return TRUE;
}