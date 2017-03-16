#pragma once
#include "DetectMemoryLeak.h"

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

void TControl::SetRect(int x1, int y1, int x2, int y2)
{
	::MoveWindow(m_hWnd, x1, y1, x2 - x1, y2 - y1, true);
}

LRESULT CALLBACK TControl::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TControl * pControl;
	pControl = (TControl *)GetWindowLong(hWnd, GWL_USERDATA);

	WNDPROC oldProc;
	oldProc = (WNDPROC)GetProp(hWnd, TEXT("oldProc"));
	if (pControl)
		return pControl->WndProc((WNDPROC)oldProc, hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc((WNDPROC)oldProc, hWnd, uMsg, wParam, lParam);
}

void TControl::RegisterProc()//创建窗口后注册
{
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	WNDPROC oldProc;
	oldProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)subControlProc);

	SetProp(m_hWnd, TEXT("oldProc"), oldProc);
}

LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//虚拟消息处理函数，可重载
{
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}