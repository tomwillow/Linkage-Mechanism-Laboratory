#pragma once
#include <Windows.h>

class KWindow
{
private:
	HACCEL    m_hAccelTable;
	bool m_bDoubleBuffer;
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
	virtual void OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//WM_SETCURSOR事件捕捉后，系统将不处理光标变化。程序若不自行设置，光标将变成“忙”光标。
		DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	virtual void OnSize(WPARAM wParam, LPARAM lParam){	}
	virtual void OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){ }
    
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void GetWndClassEx(WNDCLASSEX & wc);

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
   
	HRESULT CommonMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
		HMENU hMenu, int nWindowMenu);

	HPALETTE m_hPalette;
	int		 m_nUpdateCount;

	virtual LRESULT OnQueryNewPalette(void);
	virtual LRESULT OnPaletteChanged(HWND hWnd, WPARAM wParam);

	bool    m_bMainWindow;

public:

	HWND  m_hWnd;
	HINSTANCE m_hInst;
	RECT ClientRect;
	RECT WindowRect;
	KWindow(void)
	{
		m_hWnd		   = NULL;
		m_hPalette	   = NULL;
		m_nUpdateCount = 0;
		m_bMainWindow  = false;
		m_bDoubleBuffer = false;

	}

	virtual ~KWindow(void)
	{
		if ( m_hPalette )
		{
			DeleteObject(m_hPalette);
			m_hPalette = NULL;
		}
	}
    
	virtual bool CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle, 
		int x, int y, int nWidth, int nHeight, HWND hParent, HMENU hMenu, HINSTANCE hInst);

	bool RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst);

	virtual WPARAM MessageLoop(void);
	void SetAccel(HACCEL hAccel);
	void SetDoubleBuffer(bool bDoubleBuffer);
	void CDECL KWindow::SetText(TCHAR szFormat[], ...);
	BOOL ShowWindow(int nCmdShow) const
	{
		return ::ShowWindow(m_hWnd, nCmdShow);
	}

	BOOL UpdateWindow(void) const
	{
		return ::UpdateWindow(m_hWnd);
	}

};


void ShowMessage(TCHAR szFormat[], ...);
int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style, int iconid);
