#pragma once
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <assert.h>
#include "tchar_head.h"
#include <stdio.h>
#include "TWindow.h"


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

	case WM_DESTROY:
		if (m_bMainWindow)
			PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		if (OnClose())
			break;//Quit
		else
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK TWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TWindow * pWindow;

	if (uMsg == WM_NCCREATE)
	{
		pWindow = (TWindow *)((LPCREATESTRUCT)lParam)->lpCreateParams;
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

bool TWindow::RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst)
{
	WNDCLASSEX wc;

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


bool TWindow::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hParent,
	HMENU hMenu, HINSTANCE hInst)
{
	if (!RegisterClass(lpszClass, hInst))
		return false;

	m_hInst = hInst;
	m_hParent = hParent;
	szName = new TCHAR[_tcslen(lpszName) + 1];
	_tcscpy(szName, lpszName);

	m_hWnd = CreateWindowEx(dwExStyle, lpszClass,
		lpszName, dwStyle, x, y, nWidth, nHeight,
		hParent, hMenu, hInst, this);

	return m_hWnd != NULL;
}

void TWindow::LoadTitleIcon(HINSTANCE hInst, UINT id)
{
	m_hTitleIcon = LoadIcon(hInst, MAKEINTRESOURCE(id));
}


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
	wc.hbrBackground = (HBRUSH)(COLOR_MENU  +1);//
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
