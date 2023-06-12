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
    RECT re;
    GetClientRect(parent, &re);

    auto width = re.right - re.left;

    RECT r;

    r.left = 0;
    r.top = 23;
    r.right = width;
    r.bottom = r.top + 30;

    HWND hwnd = CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE,
        0, 23, width, 30, parent, (HMENU)IDM_TOPBAR, GetModuleHandle(NULL), NULL);


    if (!IsWindow(hwnd)) {
        throw (std::format(L"exception caught while creating window:\n{}", get_last_error()));
        return;
    }

    HFONT SegoeUI = CreateFont(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    fileNavigation = std::unique_ptr<cFileNavigation>(new cFileNavigation(parent, hwnd, SegoeUI));

    fileNavigation->rect = r;


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
            BACKBUTTON_X, 0, 30, 30, fileNavigation->hWnd, (HMENU)IDM_BACKBUTTON, GetModuleHandle(NULL), NULL);

        fileNavigation->g_hForwardButton = CreateWindowExW(0, L"BUTTON", L"FORWARD", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            FORWARDBUTTON_X, 0, 30, 30, fileNavigation->hWnd, (HMENU)IDM_FORWARDBUTTON, GetModuleHandle(NULL), NULL);

        fileNavigation->g_hSearchBar = CreateWindowExW(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
            SEARCHBOX_X, 0, fileNavigation->rect.right/2, 30, fileNavigation->hWnd, (HMENU)IDM_SEARCHBOX, GetModuleHandle(NULL), NULL);

        hwnd = fileNavigation->g_hSearchBar;

        SendMessage(hwnd, WM_SETFONT, (WPARAM)fileNavigation->font, TRUE);
        SendMessage(hwnd, EM_LIMITTEXT, MAX_PATH, TRUE);
        SetWindowText(hwnd, L"Search...");

        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(hwnd, GWLP_WNDPROC));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)fileNavigation->SearchBoxProc);

        if (!IsWindow(fileNavigation->g_hBackButton) || !IsWindow(fileNavigation->g_hSearchBar)) {
            throw (std::format(L"exception caught while creating window:\n{}", get_last_error()));
            return 0;
        }
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
            if (wParam == IDM_BACKBUTTON)
                return fileNavigation->OnRenderUndoButton(lpDrawItemStruct);
            else if (wParam == IDM_FORWARDBUTTON)
                return fileNavigation->OnRenderRedoButton(lpDrawItemStruct);
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
        DeleteObject(fileNavigation->font);
        break;

    default:
        break;
    }

    return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA), hWnd, message, wParam, lParam);

}
LRESULT CALLBACK cFileNavigation::SearchBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    std::wstring str;
    switch (message)
    {
    case WM_SETFOCUS:
        str = UI_GetWindowText(hWnd);

        EnableWindow(hWnd, TRUE);
        SetCaretPos(str.size()-1, 0);

        if (str.empty())
            break;
           
        SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)-1);


        break;
    case WM_KILLFOCUS:
        if (UI_GetWindowText(hWnd).empty()) {
            SetWindowText(hWnd, L"Search...");

        }
        break;
    case WM_KEYDOWN:
        if (wParam == (WPARAM)VK_RETURN) {

            str = UI_GetWindowText(hWnd);
            //wprintf_s(L"text: %s\n", str.c_str());
           
            if (str.empty()) {
                break;

            }
            
            if (!fs::is_directory(str)) {
                std::thread a([]()
                    {
                        MessageBoxW(NULL, L"Invalid directory!", L"Warning", MB_ICONWARNING);
                    });
                a.join();
                break;
            }

            fileExplorer->OnPopulateButtons(str);
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;
    case WM_DESTROY:
       // DeleteObject(SegoeUI);
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