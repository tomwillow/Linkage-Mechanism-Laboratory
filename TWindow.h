#pragma once
#include <Windows.h>

class TWindow
{
private:
	HICON m_hTitleIcon;//图标
	HACCEL m_hAccelTable;//快捷键
	bool m_bDoubleBuffer;//双缓冲
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//静态消息处理
protected:

	virtual void OnDraw(HDC hdc)	{	}
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam)	{	}
	virtual void OnCommand(WPARAM wParam, LPARAM lParam){	}
	virtual void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam){	}//重载此事件时不可使用m_hWnd作为窗口句柄
	virtual void OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){	}
	virtual void OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){	}
	virtual void OnMButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){	}
	virtual void OnMButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){	}
	virtual void OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){	}
	virtual void OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){	}
	virtual void OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){ }
	virtual bool OnClose(){ return true;}
	virtual void OnSize(WPARAM wParam, LPARAM lParam){	}
	virtual void OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//WM_SETCURSOR事件捕捉后，系统将不处理光标变化。程序若不自行设置，光标将变成“忙”光标。
		DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
    
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//消息处理

	virtual void GetWndClassEx(WNDCLASSEX & wc);

   
	HRESULT CommonMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
		HMENU hMenu, int nWindowMenu);

	HPALETTE m_hPalette;
	int		 m_nUpdateCount;

	virtual LRESULT OnQueryNewPalette(void);
	virtual LRESULT OnPaletteChanged(HWND hWnd, WPARAM wParam);

	bool    m_bMainWindow;

public:
	TCHAR *szName;//名称
	HWND m_hWnd;
	HWND m_hParent;
	HINSTANCE m_hInst;
	RECT ClientRect;//在WM_PAINT中自动更新
	RECT WindowRect;
	TWindow(void)
	{
		m_hParent = NULL;
		m_hWnd		   = NULL;
		m_hPalette	   = NULL;
		m_nUpdateCount = 0;
		m_bMainWindow  = false;
		m_bDoubleBuffer = false;
		m_hTitleIcon = NULL;
		szName = NULL;
	}

	virtual ~TWindow(void)
	{
		if (szName != NULL)
			delete[] szName;
		if ( m_hPalette )
		{
			DeleteObject(m_hPalette);
			m_hPalette = NULL;
		}
	}

	void TWindow::LoadTitleIcon(HINSTANCE hInst, UINT id);//先于窗口载入图标
	virtual bool CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle, 
		int x, int y, int nWidth, int nHeight, HWND hParent, HMENU hMenu, HINSTANCE hInst);

	bool RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst);

	virtual WPARAM MessageLoop();
	void SetAccel(UINT id);
	void SetDoubleBuffer(bool bDoubleBuffer);
	void CDECL TWindow::SetText(TCHAR szFormat[], ...);
	void TWindow::Invalidate();//重绘窗口
	BOOL ShowWindow(int nCmdShow) const
	{
		return ::ShowWindow(m_hWnd, nCmdShow);
	}

	BOOL UpdateWindow(void) const
	{
		return ::UpdateWindow(m_hWnd);
	}

	void TWindow::SetRect(RECT &rect)
	{
		::MoveWindow(m_hWnd, rect.top, rect.left, rect.right, rect.bottom, true);
	}

	void TWindow::SetRect(int x1, int y1, int x2, int y2)
	{
		::MoveWindow(m_hWnd, x1, y1, x2 - x1, y2 - y1, true);
	}
};


void ShowMessage(TCHAR szFormat[], ...);
int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style, int iconid);
