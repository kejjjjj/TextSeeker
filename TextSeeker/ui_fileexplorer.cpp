#include "pch.hpp"
LRESULT CALLBACK cFileExplorer::ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

			//lastMousePos.x = GET_X_LPARAM(lParam);
			//lastMousePos.y = GET_Y_LPARAM(lParam);

			GetCursorPos(&lastMousePos);

			ScreenToClient(hWnd, &lastMousePos);


			SetCapture(hWnd);

		}
		else
			isResizing = false;
		return DefWindowProc(hWnd, message, wParam, lParam);

	}
	case WM_MOUSEACTIVATE:
	case WM_MOUSEMOVE:
	{
		if (!isResizing)
			break;

		POINT currentMousePos{};
		GetCursorPos(&currentMousePos);

		ScreenToClient(hWnd, &currentMousePos);

		RECT rcWindow{}, parentWindow{};
		GetWindowRect(hWnd, &rcWindow);
		GetWindowRect(hWnd, &parentWindow);

		int dx = (currentMousePos.x - lastMousePos.x);


		int newWidth = rcWindow.right - rcWindow.left;
		
		fileExplorer->rect.left = rcWindow.left;
		fileExplorer->rect.right = newWidth + dx;
		//fileExplorer->rect.top = 0;
		//fileExplorer->rect.bottom = parentWindow.bottom - 58;
		
		SetWindowPos(hWnd, NULL, 0, 58, fileExplorer->rect.right, fileExplorer->rect.bottom, SWP_NOZORDER);

		lastMousePos = currentMousePos;

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	}
	break;
	case WM_LBUTTONUP:
		isResizing = false;

		ReleaseCapture();
		break;
		break;
	case WM_PAINT:
	{
		fileExplorer->OnPaint(wParam, lParam);
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

	fileExplorer = std::unique_ptr<cFileExplorer>(new cFileExplorer(parent, 
		CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
		0, 58, 400, height, parent, (HMENU)IDM_FILEEXPLORER, GetModuleHandle(NULL), NULL)));

	fileExplorer->rect.left = 0;
	fileExplorer->rect.right = 400;
	fileExplorer->rect.top = 58;
	fileExplorer->rect.bottom = height;


	SetWindowLongPtr(fileExplorer->hWnd, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(fileExplorer->hWnd, GWLP_WNDPROC));
	SetWindowLongPtr(fileExplorer->hWnd, GWLP_WNDPROC, (LONG_PTR)fileExplorer->ProcHandler);

}
void cFileExplorer::OnResize(uint32_t width, uint32_t height) 
{

	fileExplorer->rect.bottom = height - 58;


	SetWindowPos(hWnd, NULL, 0, 58, fileExplorer->rect.right, fileExplorer->rect.bottom, SWP_NOZORDER);
}
void cFileExplorer::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	SetBkColor(hdc, RGB(25, 25, 25));

	RECT rect;
	GetClientRect(hWnd, &rect);
	FillRect(hdc, &rect, CreateSolidBrush(RGB(25, 25, 25)));

	DrawTextOnMainWindow(hWnd);

	EndPaint(hWnd, &ps);
	return;
}