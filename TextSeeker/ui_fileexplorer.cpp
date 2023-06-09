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

		fileExplorer->OnPopulateButtons(L"\\");

		return 0;
	case WM_SIZE:
		/*fileExplorer->OnPopulateButtons();*/
		break;
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
		POINT currentMousePos{};
		GetCursorPos(&currentMousePos);

		if (!isResizing)
			break;


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

	case WM_VSCROLL:
		fileExplorer->OnVerticalScroll(wParam);

		//InvalidateRect(hWnd, NULL, TRUE);

		return 0;
	case WM_MOUSEWHEEL:
		fileExplorer->OnWheelScroll(wParam);
		return 0;
	break;
	case WM_COMMAND:
		break;

	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;

		if (lpDrawItemStruct->CtlType == ODT_BUTTON)
		{
			if(wParam >= IDM_DIRECTORY_ITEMS && wParam < IDM_DIRECTORY_ITEMS + fileExplorer->buttons.size())
				return fileExplorer->OnRenderDirectoryButton(lpDrawItemStruct);
		}
		break;
	}
	default:
		break;
	}

	return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA), hWnd, message, wParam, lParam);
}
LRESULT cFileExplorer::OnRenderDirectoryButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	auto b = fileExplorer->FindButtonByHWND(lpDrawItemStruct->hwndItem);

	if (!b) {
		MessageBox(NULL, L"failed", L"yea", MB_USERICON);
		return FALSE;
	}
	HDC hdcButton = lpDrawItemStruct->hDC;
	RECT rect = lpDrawItemStruct->rcItem;
	UINT state = lpDrawItemStruct->itemState;
	b->rect = rect;
	SetBkMode(hdcButton, TRANSPARENT);

	SetTextColor(hdcButton, b->file.is_directory ? RGB(255, 255, 255) : RGB(128, 128, 128));

	HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 25)); // Example: Gray when pressed, black otherwise


	// Fill the button rectangle with the transparent brush
	FillRect(hdcButton, &rect, hBrush);

	// Draw the icon on the left side of the button
	SHFILEINFO stFileInfo;
	SHGetFileInfo(b->file.path.c_str(),
		FILE_ATTRIBUTE_NORMAL,
		&stFileInfo,
		sizeof(stFileInfo),
		SHGFI_ICON | SHGFI_SMALLICON);

	rect.left += 6;
	DrawIcon(hdcButton, rect.left, rect.top, stFileInfo.hIcon);

	DestroyIcon(stFileInfo.hIcon);

	RECT textRect = rect;
	textRect.left += GetSystemMetrics(SM_CXICON) * 1.25;

	DrawText(hdcButton, b->file.name.c_str(), -1, &textRect, DT_SINGLELINE | DT_VCENTER);

	DeleteObject(hBrush);

	return TRUE;
}
void UI_CreateFileExplorer(HWND parent)
{
	RECT r;
	GetClientRect(parent, &r);

	r.top = EXPLORER_TOP_OFFSET;
	r.bottom = r.bottom;

	auto height = r.bottom - r.top;

	fileExplorer = std::unique_ptr<cFileExplorer>(new cFileExplorer(parent, 
		CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
		0, EXPLORER_TOP_OFFSET, 250, height, parent, (HMENU)IDM_FILEEXPLORER, GetModuleHandle(NULL), NULL)));

	fileExplorer->rect.left = 0;
	fileExplorer->rect.right = 250;
	fileExplorer->rect.top = EXPLORER_TOP_OFFSET;
	fileExplorer->rect.bottom = height;

	

	//fileExplorer->current_directory.resize(MAX_PATH);

	//SHGetFolderPathW(NULL, CSIDL_MYCOMPUTER, NULL, 0, fileExplorer->current_directory.data());


	SetWindowLongPtr(fileExplorer->hWnd, GWLP_USERDATA, (LONG_PTR)GetWindowLongPtr(fileExplorer->hWnd, GWLP_WNDPROC));
	SetWindowLongPtr(fileExplorer->hWnd, GWLP_WNDPROC, (LONG_PTR)fileExplorer->ProcHandler);

	SendMessage(fileExplorer->hWnd, WM_CREATE, NULL, NULL);

}
void cFileExplorer::OnResize(uint32_t width, uint32_t height) 
{

	fileExplorer->rect.bottom = height - EXPLORER_TOP_OFFSET;


	SetWindowPos(hWnd, NULL, 0, EXPLORER_TOP_OFFSET, fileExplorer->rect.right, fileExplorer->rect.bottom, SWP_NOZORDER);
}
void cFileExplorer::OnCreateScroll(uint32_t numLines)
{


	uint32_t width = rect.right - rect.left;
	uint32_t height = rect.bottom - rect.top - 58;

	SCROLLINFO si;

	// Set the vertical scrolling range and page size
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = numLines - 1;
	si.nPage = height / sScroll.nPage/3;
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

	//// Set the horizontal scrolling range and page size. 
	//si.cbSize = sizeof(si);
	//si.fMask = SIF_RANGE | SIF_PAGE;
	//si.nMin = 0;
	//si.nMax = 2 + sScroll.xMax / sScroll.horzScroll;
	//si.nPage = width / sScroll.horzScroll;
	//SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

}
void cFileExplorer::OnVerticalScroll(WPARAM wParam)
{
	SCROLLINFO si;
	ScrollMetrics& scroll = sScroll;
	
	// Get all the vertial scroll bar information.
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(hWnd, SB_VERT, &si);

	// Save the position for comparison later on.
	scroll.scrollY = si.nPos;
	switch (LOWORD(wParam))
	{

		// User clicked the HOME keyboard key.
	case SB_TOP:
		si.nPos = si.nMin;
		break;

		// User clicked the END keyboard key.
	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;

		// User clicked the top arrow.
	case SB_LINEUP:
		si.nPos -= 1;
		break;

		// User clicked the bottom arrow.
	case SB_LINEDOWN:
		si.nPos += 1;
		break;

		// User clicked the scroll bar shaft above the scroll box.
	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;

		// User clicked the scroll bar shaft below the scroll box.
	case SB_PAGEDOWN:
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
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
	GetScrollInfo(hWnd, SB_VERT, &si);

	auto _rect = rect;
	_rect.top -= 45;

	// If the position has changed, scroll window and update it.
	if (si.nPos != scroll.scrollY)
	{
		ScrollWindow(hWnd, 0, scroll.vertScroll * (scroll.scrollY - si.nPos), NULL, &_rect);
		UpdateWindow(hWnd);
		
	}

}
void cFileExplorer::OnWheelScroll(WPARAM wParam)
{
	int nScrollLines = 3; 
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	int nVScrollAmount = (zDelta / WHEEL_DELTA) * nScrollLines;

	SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, nVScrollAmount);
}
sExplorerButton* cFileExplorer::FindButtonByHWND(HWND hWnd)
{
	for (auto& i : buttons) {
		if (i.hWnd == hWnd)
			return &i;
	}
	return 0;
}
sExplorerButton* cFileExplorer::GetHoveredButton(POINT mouse)
{

	ScreenToClient(hWnd, &mouse);

	std::cout << "x: " << mouse.x << "\ny: " << mouse.y << '\n';

	for (auto& i : buttons) {
		if (isHovered(i.rect, &mouse)) {
			
			return &i;
		}
		
	}
	return 0;

}
void cFileExplorer::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	SCROLLINFO si;
	TEXTMETRIC tm;


	auto& scroll = fileExplorer->sScroll;

	GetTextMetrics(hdc, &tm);

	scroll.horzScroll = tm.tmAveCharWidth / 1.5;
	scroll.upperWidth = (tm.tmPitchAndFamily & 1 ? 3 : 2) * scroll.horzScroll / 2;
	//scroll.vertScroll = (tm.tmHeight);

	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hWnd, SB_VERT, &si);
	scroll.scrollY = si.nPos;

	//GetScrollInfo(hWnd, SB_HORZ, &si);
	//scroll.scrollX = si.nPos;


	OnCreateScroll(buttons.size());


	SetBkColor(hdc, RGB(25, 25, 25));
	
	RECT rect;
	GetClientRect(hWnd, &rect);
	auto brush = CreateSolidBrush(RGB(25, 25, 25));
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);

	EndPaint(hWnd, &ps);
	return;
}

void cFileExplorer::OnPopulateButtons(const std::wstring& loc)
{

	SCROLLINFO si;
	TEXTMETRIC tm;
	RECT r;
	SHFILEINFO stFileInfo;
	HDC hdc;
	ScrollMetrics& scroll = fileExplorer->sScroll;
	std::vector<sFile> files;
	UINT hmenu = IDM_DIRECTORY_ITEMS;
	HWND hwnd;
	HFONT segoeUI;

	hdc = GetDC(hWnd);
	GetTextMetrics(hdc, &tm);

	auto copy = loc;

	scroll.horzScroll = tm.tmAveCharWidth / 1.5;
	scroll.upperWidth = (tm.tmPitchAndFamily & 1 ? 3 : 2) * scroll.horzScroll / 2;
	scroll.vertScroll = /*(tm.tmHeight + tm.tmExternalLeading) + */45;
	scroll.scrollY = 0;

	UINT a = IDM_DIRECTORY_ITEMS;
	for (auto& i : buttons) {
		RemoveWindowSubclass(i.hWnd, OwnerDrawButtonProc, a++);
		i.Destroy(); //kill window & delete font
	}

	buttons.clear();

	GetClientRect(hWnd, &r);

	r.top = 6;
	if (fs::files_in_directory(copy, files)) {
		//auto longest = std::max_element(files.begin(), files.end(), [](const sFile& a, const sFile& b) {return a.name.size() < b.name.size(); });
		scroll.nPage = (tm.tmHeight);
		for (auto it = files.begin(); it != files.end(); ++it) {
			std::wcout << it->name << '\n';

			r.left = 6;
			
			r.right = r.left + rect.right;
			r.bottom = r.top + 30;
			

			segoeUI = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

			hwnd = CreateWindowExW(0, L"BUTTON", it->name.c_str(), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				0, r.top, rect.right, 30, hWnd, (HMENU)hmenu, GetModuleHandle(NULL), NULL);
			SetWindowSubclass(hwnd, &OwnerDrawButtonProc, hmenu, 0);
			buttons.push_back(
				{
					hwnd,
					hmenu,
					*it,
					r,
					segoeUI
				});

			r.top += 45;

			SelectObject(hdc, segoeUI);


			SendMessage(hwnd, WM_SETFONT, (WPARAM)segoeUI, TRUE);
			++hmenu;
		}
		//std::wcout << L"longest elem: " << longest->name << '\n';
		//scroll.xMax = (int)longest->name.length() * scroll.horzScroll;

		

	}

	ReleaseDC(hWnd, hdc);
}


LRESULT CALLBACK cFileExplorer::OwnerDrawButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)

{
	sExplorerButton* b = 0;
	HCURSOR hCursor;

	b = fileExplorer->FindButtonByHWND(hWnd);

	if (!b)
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		

		b->file.is_directory ? SetCursor(LoadCursor(NULL, IDC_HAND)) : SetCursor(LoadCursor(NULL, IDC_NO));


		break;
	case WM_LBUTTONDOWN:
		

		if (!b->file.is_directory) {
			//MessageBox(NULL, L"Item must be a folder", L"Error!", MB_ICONERROR);
			break;
		}

		fileExplorer->OnPopulateButtons(b->file.path);

		std::wcout << b->file.path << '\n';
		break;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, &OwnerDrawButtonProc, 1);
		break;
	default:
		break;
	}


	return DefSubclassProc(hWnd, uMsg, wParam, lParam);


}