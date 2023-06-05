#include "pch.hpp"

LRESULT CALLBACK FileExplorerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT lastMousePos{};
	static bool isResizing = false;

	switch (message)
	{
	case WM_NCHITTEST:
	{
		POINT pt{};
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		ScreenToClient(hWnd, &pt);

		RECT rcClient{};
		GetClientRect(hWnd, &rcClient);

		const int gripSize = 8; 
		const int leftMargin = 10; 

		RECT rcLeftResize{};
		rcLeftResize.left = rcClient.left + leftMargin;
		rcLeftResize.top = rcClient.top;
		rcLeftResize.right = rcClient.left + leftMargin + gripSize;
		rcLeftResize.bottom = rcClient.bottom;

		RECT rcRightResize{};
		rcRightResize.left = rcClient.right - gripSize;
		rcRightResize.top = rcClient.top;
		rcRightResize.right = rcClient.right;
		rcRightResize.bottom = rcClient.bottom;

		if (PtInRect(&rcLeftResize, pt))
			return HTLEFT;
		else if (PtInRect(&rcRightResize, pt))
			return HTRIGHT; 

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_NCLBUTTONDOWN:
	{

		if (wParam == HTLEFT || wParam == HTRIGHT)
		{
			isResizing = true;

			lastMousePos.x = GET_X_LPARAM(lParam);
			lastMousePos.y = GET_Y_LPARAM(lParam);

			SetCapture(hWnd);
			
		}
		else
			isResizing = false;
		return DefWindowProc(hWnd, message, wParam, lParam);
		
	}
	case WM_MOUSEACTIVATE:
	case WM_MOUSEMOVE:
	{
		std::boolalpha;
		if (isResizing) {
			POINT currentMousePos{};
			currentMousePos.x = GET_X_LPARAM(lParam);
			currentMousePos.y = GET_Y_LPARAM(lParam);

			ScreenToClient(hWnd, &currentMousePos);

			RECT rcWindow{};
			GetWindowRect(hWnd, &rcWindow);

			int dx = (currentMousePos.x - lastMousePos.x) * 100;
			std::cout << "dx:" << dx << "\n";

			int newWidth = rcWindow.right - rcWindow.left;
			if (wParam == HTLEFT)
				newWidth -= dx;
			else if (wParam == HTRIGHT)
				newWidth += dx;

			SetWindowPos(hWnd, NULL, 0, 0, newWidth, rcWindow.bottom - rcWindow.top, SWP_NOMOVE | SWP_NOZORDER);

			lastMousePos = currentMousePos;

			InvalidateRect(g_hMainWnd, NULL, TRUE);
			break;
		}
	}
	break;
	case WM_LBUTTONUP:
		isResizing = false;

		ReleaseCapture();
		break;
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// Set the desired background color
		SetBkColor(hdc, RGB(25, 25, 25));

		// Paint the entire client area with the background color
		RECT rect;
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, CreateSolidBrush(RGB(25, 25, 25)));

		DrawTextOnMainWindow(hWnd);

		EndPaint(hWnd, &ps);
		return 0;
	}
	break;
	default:
		break;
	}

	return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA), hWnd, message, wParam, lParam);

}


void UI_CreateFileExplorer(HWND parent)
{
	RECT r;
	GetClientRect(parent, &r);

	r.top = 58;
	r.bottom = r.bottom;

	auto height = r.bottom - r.top;

	g_hFileExplorer = CreateWindowExA(0, "STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
		0, 58, 400, height, parent, (HMENU)IDM_FILEEXPLORER, GetModuleHandle(NULL), NULL);

	SetWindowLongPtr(g_hFileExplorer, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(g_hFileExplorer, GWLP_WNDPROC));
	SetWindowLongPtr(g_hFileExplorer, GWLP_WNDPROC, (LONG_PTR)FileExplorerProc);

}

void UI_ResizeFileExplorer(uint32_t width, uint32_t height)
{
	SetWindowPos(g_hFileExplorer, NULL, 0, 58, width * 0.25f, height, SWP_NOMOVE | SWP_NOZORDER);
}