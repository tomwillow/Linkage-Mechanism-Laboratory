#pragma once
#include <Windows.h>

class TControl
{
public:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TControl();
	~TControl();
	void TControl::SetRect(RECT &rect);
	void TControl::SetRect(int x1, int y1, int x2, int y2);
};

