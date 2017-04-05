#pragma once
#include <Windows.h>

class TControl
{
private:
	static LRESULT CALLBACK TControl::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	LONG m_iWidth, m_iHeight;
	HWND m_hParent;
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TControl();
	~TControl();
	void TControl::LinkControl(HWND hwnd);
	void TControl::SetRect(RECT &rect);
	void TControl::SetRect(int x1, int y1, int x2, int y2); 
	void TControl::SetPos(RECT &rect);
protected:
	void TControl::RegisterProc();//创建窗口后注册

	virtual LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//虚拟消息处理函数，可覆盖

};
