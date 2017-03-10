#pragma once
#include <Windows.h>

class TControl
{
private:
	static WNDPROC oldControlProc;
	static LRESULT CALLBACK subControlProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TControl();
	~TControl();
	void TControl::SetRect(RECT &rect);
	void TControl::SetRect(int x1, int y1, int x2, int y2);
protected:
	void TControl::RegisterProc();//创建窗口后注册

	//虚拟消息处理函数，可重载
	virtual LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

