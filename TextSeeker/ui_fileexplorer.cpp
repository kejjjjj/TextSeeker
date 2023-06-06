#include "pch.hpp"
LRESULT CALLBACK cFileExplorer::ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT lastMousePos{};
	static bool isResizing = false;
	HDC hdc;
	TEXTMETRIC tm;
	auto& scroll = fileExplorer->sScroll;
	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd);

		GetTextMetrics(hdc, &tm);

		

		scroll.horzScroll = tm.tmAveCharWidth;
		scroll.upperWidth = (tm.tmPitchAndFamily & 1 ? 3 : 2) * scroll.horzScroll / 2;
		scroll.vertScroll = tm.tmHeight + tm.tmExternalLeading;

		scroll.xMax = 48 * scroll.horzScroll+ 12 * scroll.upperWidth;


		ReleaseDC(hWnd, hdc);

		return 0;
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
		//fileExplorer->rect.bottom = parentWindow.bottom - EXPLORER_TOP_OFFSET;
		
		SetWindowPos(hWnd, NULL, 0, EXPLORER_TOP_OFFSET, fileExplorer->rect.right, fileExplorer->rect.bottom, SWP_NOZORDER);

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

	case WM_HSCROLL:
		fileExplorer->OnHorizontalScroll(wParam);

		//InvalidateRect(hWnd, NULL, TRUE);

		return 0;
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

	r.top = EXPLORER_TOP_OFFSET;
	r.bottom = r.bottom;

	auto height = r.bottom - r.top;

	fileExplorer = std::unique_ptr<cFileExplorer>(new cFileExplorer(parent, 
		CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL,
		0, EXPLORER_TOP_OFFSET, 400, height, parent, (HMENU)IDM_FILEEXPLORER, GetModuleHandle(NULL), NULL)));

	fileExplorer->rect.left = 0;
	fileExplorer->rect.right = 400;
	fileExplorer->rect.top = EXPLORER_TOP_OFFSET;
	fileExplorer->rect.bottom = height;


	SetWindowLongPtr(fileExplorer->hWnd, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(fileExplorer->hWnd, GWLP_WNDPROC));
	SetWindowLongPtr(fileExplorer->hWnd, GWLP_WNDPROC, (LONG_PTR)fileExplorer->ProcHandler);

}
void cFileExplorer::OnResize(uint32_t width, uint32_t height) 
{

	fileExplorer->rect.bottom = height - EXPLORER_TOP_OFFSET;


	SetWindowPos(hWnd, NULL, 0, EXPLORER_TOP_OFFSET, fileExplorer->rect.right, fileExplorer->rect.bottom, SWP_NOZORDER);
}
void cFileExplorer::OnCreateScroll(uint32_t numLines)
{


	uint32_t width = rect.right - rect.left;
	uint32_t height = rect.bottom - rect.top;

	SCROLLINFO si;

	// Set the vertical scrolling range and page size
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = numLines - 1;
	si.nPage = height / sScroll.vertScroll;
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

	// Set the horizontal scrolling range and page size. 
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = 2 + sScroll.xMax / sScroll.horzScroll;
	si.nPage = width / sScroll.horzScroll;
	SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

}
void cFileExplorer::OnHorizontalScroll(WPARAM wParam)
{
	SCROLLINFO si;
	ScrollMetrics& scroll = sScroll;
	
	// Get all the vertial scroll bar information.
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	// Save the position for comparison later on.
	GetScrollInfo(hWnd, SB_HORZ, &si);
	scroll.scrollX = si.nPos;

	switch (LOWORD(wParam))
	{
		// User clicked the left arrow.
	case SB_LINELEFT:
		si.nPos -= 1;
		break;

		// User clicked the right arrow.
	case SB_LINERIGHT:
		si.nPos += 1;
		break;

		// User clicked the scroll bar shaft left of the scroll box.
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		break;

		// User clicked the scroll bar shaft right of the scroll box.
	case SB_PAGERIGHT:
		si.nPos += si.nPage;
		break;

		// User dragged the scroll box.
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	default:
		break;
	}

	// Set the position and then retrieve it.  Due to adjustments
	   // by Windows it may not be the same as the value set.
	si.fMask = SIF_POS;
	SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
	GetScrollInfo(hWnd, SB_HORZ, &si);

	// If the position has changed, scroll the window.
	if (si.nPos != scroll.scrollX)
	{
		ScrollWindow(hWnd, scroll.horzScroll * (scroll.scrollX - si.nPos), 0, NULL, NULL);
		UpdateWindow(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);

	}

}
void cFileExplorer::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	SCROLLINFO si;

	TEXTMETRIC tm;
	auto& scroll = fileExplorer->sScroll;

	// Get vertical scroll bar position.
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hWnd, SB_VERT, &si);
	scroll.scrollY = si.nPos;

	// Get horizontal scroll bar position.
	GetScrollInfo(hWnd, SB_HORZ, &si);
	scroll.scrollX = si.nPos;



	GetTextMetrics(hdc, &tm);



	scroll.horzScroll = tm.tmAveCharWidth / 3.2;
	scroll.upperWidth = (tm.tmPitchAndFamily & 1 ? 3 : 2) * scroll.horzScroll / 2;
	scroll.vertScroll = (tm.tmHeight + tm.tmExternalLeading) * 1.5;

	scroll.xMax = 256 * scroll.horzScroll + 12 * scroll.upperWidth;

	SetBkColor(hdc, RGB(25, 25, 25));

	RECT rect;
	GetClientRect(hWnd, &rect);
	FillRect(hdc, &rect, CreateSolidBrush(RGB(25, 25, 25)));

	std::vector<sFile> files;
	if (fs::files_in_directory(L"C:\\", files)) {
		OnCreateScroll(files.size());
		rect.left = scroll.horzScroll * (1 - scroll.scrollX);

		for (auto& i : files) {
			DrawTextOnWindow(hWnd, i.path.c_str(), rect, RGB(255, 255, 255), 20, FALSE, FALSE);
			rect.top += 25;
		}
	}

	EndPaint(hWnd, &ps);
	return;
}