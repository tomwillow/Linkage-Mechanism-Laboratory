#pragma once
#include "..\Control\TEdit.h"
#include "..\Draw\TDraw.h"

#include <CommCtrl.h>
TEdit::TEdit()
{

	bMultiLine = false;
	bAutoHScrol = false;
	bAutoVScrol = false;
	bNoHideSel = false;
}

TEdit::~TEdit()
{
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
	m_hParent = hParent;
	m_hWnd = ::CreateWindowEx(dwStyle,//WS_EX_CLIENTEDGE
		TEXT("Edit"), 0,
		WS_CHILD |
		(bMultiLine ? ES_MULTILINE : 0) |
		(bAutoHScrol ? ES_AUTOHSCROLL|WS_HSCROLL : 0)|
		(bAutoVScrol?ES_AUTOVSCROLL|WS_VSCROLL:0)|
		(bNoHideSel?ES_NOHIDESEL:0)| WS_VISIBLE,//|ES_RIGHT 
		0, 0, 0, 0, hParent,(HMENU)id, hInst,0);

	RegisterProc();
}



void TEdit::SetSelect(int iStart, int iEnd)
{
	::SendMessage(m_hWnd, EM_SETSEL, iStart, iEnd);
}

void TEdit::SetSelectAll()
{
	SetSelect(0, GetLength());
}
