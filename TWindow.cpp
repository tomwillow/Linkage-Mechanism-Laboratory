#pragma once
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <assert.h>
#include "tchar_head.h"
#include <stdio.h>
#include "TWindow.h"


// Default message handler for main program window, dispatch to OnKeyDown, OnDraw, etc.
LRESULT TWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		GetWindowRect(hWnd, &WindowRect);
		GetClientRect(hWnd, &ClientRect);
		OnCreate(hWnd, wParam, lParam);
		return 0;
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		return 0;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return 0;
	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_RBUTTONDOWN:
		OnRButtonDown(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_RBUTTONUP:
		OnRButtonUp(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_HOTKEY:
		OnHotKey(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_MBUTTONDOWN:
		OnMButtonDown(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_MBUTTONUP:
		OnMButtonUp(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_MOUSEWHEEL:
		OnMouseWheel(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_SETCURSOR:
		OnSetCursor(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_MOVE:
		GetWindowRect(m_hWnd, &WindowRect);
		return 0;
	case WM_SIZE:
		GetWindowRect(m_hWnd, &WindowRect);
		GetClientRect(m_hWnd, &ClientRect);
		OnSize(wParam, lParam);
		return 0;
	case WM_NOTIFY:
		OnNotify(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_ERASEBKGND:
		if (m_bDoubleBuffer == false)
			DefWindowProc(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &ClientRect);

		if (m_bDoubleBuffer)
		{
			HDC hDCMem;
			HBITMAP hBitmap;
			hDCMem = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top);
			SelectObject(hDCMem, hBitmap);

			//toDo:
			OnDraw(hDCMem);

			BitBlt(hdc, 0, 0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, hDCMem, 0, 0, SRCCOPY);
			DeleteObject(hBitmap);
			DeleteDC(hDCMem);
		}
		else
			OnDraw(ps.hdc);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_PALETTEISCHANGING: // should not happen
		MessageBox(NULL, _T("Hello"), _T("Hi"), MB_OK);
		return 0;

	case WM_PALETTECHANGED:
		return OnPaletteChanged(hWnd, wParam);

	case WM_QUERYNEWPALETTE:
		return OnQueryNewPalette();

	case WM_DESTROY:
		if (m_bMainWindow)
			PostQuitMessage(0); // main window only
		return 0;
	case WM_CLOSE:
		if (OnClose())
			break;//Quit
		else
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// Generic window procedure passed to WIN32 API, dispatches to TWindow::WndProc
LRESULT CALLBACK TWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TWindow * pWindow;

	if (uMsg == WM_NCCREATE)
	{
		MDICREATESTRUCT * pMDIC = (MDICREATESTRUCT *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		pWindow = (TWindow *)(pMDIC->lParam);
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)pWindow);
	}
	else
		pWindow = (TWindow *)GetWindowLong(hWnd, GWL_USERDATA);


	if (pWindow)
	{
#ifdef _DEBUG
		//TCHAR temp[MAX_PATH], name[MAX_PATH];

		////GetWindowText(hWnd, name, MAX_PATH);
		//wsprintf(temp, TEXT("Message(0x%x, 0x%x, 0x%x, 0x%x)\n"),hWnd, uMsg,wParam,lParam);
		//OutputDebugString(temp);
#endif
		return pWindow->WndProc(hWnd, uMsg, wParam, lParam);

	}
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// Register WNDCLASS for the window class. Registering only once
bool TWindow::RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst)
{
	WNDCLASSEX wc;

	// Check if class is already registered
	wc.cbSize = sizeof(wc);

	if (!GetClassInfoEx(hInst, lpszClass, &wc))
	{
		GetWndClassEx(wc);				// fill class attributes

		wc.hInstance = hInst;
		wc.lpszClassName = lpszClass;

		if (!RegisterClassEx(&wc))	// register
			return false;
	}

	return true;
}


// Handles window creation
bool TWindow::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hParent,
	HMENU hMenu, HINSTANCE hInst)
{
	if (!RegisterClass(lpszClass, hInst))
		return false;

	// Use MDICREATESTRUCT to support MDI child window
	MDICREATESTRUCT mdic;
	memset(&mdic, 0, sizeof(mdic));
	mdic.lParam = (LPARAM) this;

	m_hInst = hInst;
	m_hParent = hParent;
	szName = new TCHAR[_tcslen(lpszName) + 1];
	_tcscpy(szName, lpszName);
	m_hWnd = CreateWindowEx(dwExStyle, lpszClass,
		lpszName, dwStyle, x, y, nWidth, nHeight,
		hParent, hMenu, hInst, &mdic);

	return m_hWnd != NULL;
}

void TWindow::LoadTitleIcon(HINSTANCE hInst, UINT id)
{
	m_hTitleIcon = LoadIcon(hInst, MAKEINTRESOURCE(id));
}

// Fill WNDCLASSEX, virtual function
void TWindow::GetWndClassEx(WNDCLASSEX & wc)
{
	memset(&wc, 0, sizeof(wc));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0; // CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = m_hTitleIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = NULL;
	wc.hIconSm = NULL;
}


WPARAM TWindow::MessageLoop(void)
{
	MSG msg;

	m_bMainWindow = true;

	while (GetMessage(&msg, NULL, 0, 0))
	{
#ifdef _DEBUG
		TCHAR temp[MAX_PATH];//
		wsprintf(temp, TEXT("Message(0x%x, 0x%x, 0x%x, 0x%x)\n"), msg.hwnd, msg.message, msg.wParam, msg.lParam);
		//OutputDebugString(temp);
#endif
		if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}


// Common message processing for MDI Child Window
HRESULT TWindow::CommonMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	HMENU hMenu, int nWindowMenu)
{
	switch (uMsg)
	{
	case WM_NCDESTROY:						   // should be the last message
		SetWindowLong(hWnd, GWL_USERDATA, 0);  // make sure no more message through WindowProc
		delete this;						   // MDI child are created using new operator, time to delete	
		return 0;

	case WM_MDIACTIVATE:
		if (lParam == (LPARAM)hWnd)		   // if current window activate, switch menu
			SendMessage(GetParent(hWnd), WM_MDISETMENU, (WPARAM)hMenu, (LPARAM)GetSubMenu(hMenu, nWindowMenu));

		// send message to parent window to response to child window change
		SendMessage(GetParent(GetParent(hWnd)), WM_USER, lParam != (LPARAM)hWnd, 0);
		return 0;

	default:
		// generic MDI child window message handling provided by OS
		return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
	}
}


LRESULT TWindow::OnQueryNewPalette(void)
{
	if (m_hPalette == NULL)
		return FALSE;

	HDC      hDC = GetDC(m_hWnd);
	HPALETTE hOld = SelectPalette(hDC, m_hPalette, FALSE);

	BOOL changed = RealizePalette(hDC) != 0;
	SelectPalette(hDC, hOld, FALSE);
	ReleaseDC(m_hWnd, hDC);

	if (changed)
	{
		OutputDebugString(_T("InvalidateRect\n"));
		InvalidateRect(m_hWnd, NULL, TRUE); // repaint
	}

	return changed;
}


LRESULT TWindow::OnPaletteChanged(HWND hWnd, WPARAM wParam)
{
	if ((hWnd != (HWND)wParam) && m_hPalette)
	{
		HDC hDC = GetDC(hWnd);
		HPALETTE hOld = SelectPalette(hDC, m_hPalette, FALSE);

		if (RealizePalette(hDC))
			if (m_nUpdateCount >= 2)
			{
				InvalidateRect(hWnd, NULL, TRUE);
				m_nUpdateCount = 0;
			}
			else
			{
				UpdateColors(hDC);
				m_nUpdateCount++;
			}

		SelectPalette(hDC, hOld, FALSE);
		ReleaseDC(hWnd, hDC);
	}

	return 0;
}

void TWindow::SetAccel(UINT id)
{
	m_hAccelTable = LoadAccelerators(m_hInst, (LPCTSTR)id);
}

void TWindow::SetDoubleBuffer(bool bDoubleBuffer)
{
	m_bDoubleBuffer = bDoubleBuffer;
}

void CDECL TWindow::SetText(TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	SetWindowText(m_hWnd, szBuffer);
}

void TWindow::Invalidate()
{
	InvalidateRect(m_hWnd, &ClientRect, FALSE);
}
