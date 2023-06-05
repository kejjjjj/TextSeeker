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
void CreateSearchBox(HWND parent)
{
    RECT r;
    GetClientRect(parent, &r);

    auto width = r.right - r.left;

    g_hSearchBox = CreateWindowExA(0, "EDIT", "aaa", WS_CHILD | WS_VISIBLE | WS_BORDER,
        23, 23, width * 0.6666f, 30, parent, (HMENU)IDM_SEARCHBOX, GetModuleHandle(NULL), NULL);

    SegoeUI = CreateFont(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");


    // Center the text vertically
    RECT rect{};

    rect.top += 10;
    rect.bottom += 10;
    rect.left = 0;
    rect.right = 0;

   

    SendMessage(g_hSearchBox, WM_SETFONT, (WPARAM)SegoeUI, TRUE);
    SendMessage(g_hSearchBox, EM_SETRECT, TRUE, (LPARAM)&rect);
    SendMessage(g_hSearchBox, EM_LIMITTEXT, MAX_PATH, TRUE);
    SetWindowText(g_hSearchBox, L"Search...");

    SetWindowLongPtr(g_hSearchBox, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(g_hSearchBox, GWLP_WNDPROC));
    SetWindowLongPtr(g_hSearchBox, GWLP_WNDPROC, (LONG_PTR)SearchBoxProc);
}
LRESULT CALLBACK SearchBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    std::wstring str;
    switch (message)
    {
    case WM_SETFOCUS:
        SetWindowText(g_hSearchBox, L"");
        break;
    case WM_KILLFOCUS:
        if (GetWindowTextLength(g_hSearchBox) == 0) {
            SetWindowText(g_hSearchBox, L"Search...");

        }
        break;

    case WM_KEYDOWN:

        if (wParam == (WPARAM)VK_RETURN) {
            str = UI_GetWindowText(g_hSearchBox);
            if (!str.empty())
                std::wcout << "text: " << str << '\n';
        }
        break;

    case WM_DESTROY:
        DeleteObject(SegoeUI);
        break;
    }
    return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA), hWnd, message, wParam, lParam);

}