#pragma once
#include "TEdit.h"

#include <CommCtrl.h>
TEdit::TEdit()
{
	bVisible = false;
	m_hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	bMultiLine = false;
	bAutoHScrol = false;
	bAutoVScrol = false;
	bNoHideSel = false;
}

TEdit::~TEdit()
{
	::DeleteObject(m_hFont);
}

LRESULT TEdit::WndProc(WNDPROC wndproc,HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CHAR:
		if (OnChar(wParam, lParam))
			return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
		else
			return 0;
	case WM_KEYDOWN:
		MessageBeep(0);
		if (OnKeyDown(wParam, lParam))
			return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
		else
			return 0;
	case WM_KILLFOCUS:
		if (OnKillFocus(wParam, lParam))
			return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
		else
			return 0;
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TEdit::CreateEditEx(HWND hParent, UINT id, HINSTANCE hInst, DWORD dwStyle)
{
	m_hInst = hInst;
	m_hWndParent = hParent;
	m_hWnd = ::CreateWindowEx(dwStyle,//WS_EX_CLIENTEDGE
		TEXT("Edit"), 0,
		WS_CHILD |
		(bMultiLine ? ES_MULTILINE : 0) |
		(bAutoHScrol ? ES_AUTOHSCROLL|WS_HSCROLL : 0)|
		(bAutoVScrol?ES_AUTOVSCROLL|WS_VSCROLL:0)|
		(bNoHideSel?ES_NOHIDESEL:0),//|ES_RIGHT | WS_VISIBLE
		0, 0, 0, 0, hParent,(HMENU)id, hInst,0);

	RegisterProc();
}

void TEdit::SetDefaultGuiFont()
{
	SetFont(m_hFont);
}

void TEdit::SetFont(HFONT hFont)
{
	SendMessage(m_hWnd,             // Handle of edit control
		WM_SETFONT,         // Message to change the font
		(WPARAM)hFont,     // handle of the font
		MAKELPARAM(TRUE, 0) // Redraw text
		);
}

void TEdit::SetPos(int x, int y, int width, int height)
{
	::SetWindowPos(m_hWnd, HWND_TOP, x, y, width, height, 0);//SWP_SHOWWINDOW
}

//rect中各值均为坐标
void TEdit::SetPos(RECT rect)
{
	::SetWindowPos(m_hWnd, HWND_TOP, rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top, 0);//SWP_SHOWWINDOW
}

void TEdit::SetVisible(bool bShow)
{
	bVisible = bShow;
	::ShowWindow(m_hWnd, bShow ? SW_SHOWNORMAL : SW_HIDE);
}

bool TEdit::GetVisible()
{
	return bVisible;
}

int TEdit::GetLength()
{
	return ::GetWindowTextLength(m_hWnd);
}

void TEdit::SetSelect(int iStart, int iEnd)
{
	::SendMessage(m_hWnd, EM_SETSEL, iStart, iEnd);
}

void TEdit::SetSelectAll()
{
	SetSelect(0, GetLength());
}

void CDECL TEdit::SetText(TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	::SetWindowText(m_hWnd, szBuffer);
}

void TEdit::GetText(TCHAR text[])
{
	::GetWindowText(m_hWnd, text, GetLength()+1);//不知道为什么要加1才取得全
}
