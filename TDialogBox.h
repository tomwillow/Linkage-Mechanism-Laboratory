#pragma once

#include <windows.h>

class CMyDialogBox
{
public:
	CMyDialogBox(WORD wResID, UINT uMenuID, LPCTSTR szTitle);
public:
	virtual ~CMyDialogBox(void);
public:
	int ShowModalDialog(HINSTANCE hInst, HWND hwndParent);
protected:
	WORD m_wResID;
	static BOOL DialogFunc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL InitDialog(HWND hwnd);
	HWND m_hwnd;
	HINSTANCE m_hInstance;
	UINT m_uMenuID;
	HWND m_hwndMenu;
	LPCTSTR m_szTitle;
	virtual BOOL InitComponents(void) = 0;
	virtual BOOL OnCommand(WORD wNotifyCode, WORD wID, HWND hwndCtrl) = 0;
};