#include "TControl.h"


TControl::TControl()
{
}


TControl::~TControl()
{
}

void TControl::SetRect(RECT &rect)
{
	::MoveWindow(m_hWnd, rect.top, rect.left, rect.right, rect.bottom, true);
}

void TControl::SetRect(int x1,int y1,int x2,int y2)
{
	::MoveWindow(m_hWnd,x1,y1,x2-x1,y2-y1, true);
}

WNDPROC TControl::oldControlProc;
LRESULT CALLBACK TControl::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TControl * pControl;
	pControl = (TControl *)GetWindowLong(hWnd, GWL_USERDATA);
	if (pControl)
		return pControl->WndProc(oldControlProc, hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc(oldControlProc, hWnd, uMsg, wParam, lParam);
}

LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TControl::RegisterProc()
{
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	oldControlProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)subControlProc);
}