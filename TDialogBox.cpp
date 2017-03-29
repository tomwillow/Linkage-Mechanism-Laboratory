
#include "TDialogBox.h"

//#include <tpcshell.h>
//#pragma comment(lib, "aygshell.lib")

CMyDialogBox::CMyDialogBox(WORD wResID, UINT uMenuID, LPCTSTR szTitle)
	: m_uMenuID(0)
{
	this->m_wResID = wResID;
	this->m_uMenuID = uMenuID;
	this->m_szTitle = szTitle;
}

CMyDialogBox::~CMyDialogBox(void)
{
}

int CMyDialogBox::ShowModalDialog(HINSTANCE hInst, HWND hwndParent)
{
	this->m_hInstance = hInst;
	return DialogBoxParam(
		hInst,
		MAKEINTRESOURCE(m_wResID),
		hwndParent,
		(DLGPROC)DialogFunc,
		(LPARAM)this);
}

BOOL CMyDialogBox::DialogFunc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bProcessedMsg = TRUE;
	CMyDialogBox * this_ptr = (CMyDialogBox *)GetWindowLong(hWnd, DWL_USER);

	switch (Msg) {
	case WM_INITDIALOG:
		this_ptr = (CMyDialogBox *)lParam;
		SetWindowLong(hWnd, DWL_USER, (LONG)this_ptr);
		if (FALSE == this_ptr->InitDialog(hWnd)) {
			EndDialog(hWnd, -1);
		}
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE != LOWORD(wParam)) {
			SetFocus(hWnd);
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	case WM_COMMAND:
		bProcessedMsg = this_ptr->OnCommand(
			HIWORD(wParam),
			LOWORD(wParam),
			(HWND)lParam);
		break;
	default:
		bProcessedMsg = FALSE;
	}
	return bProcessedMsg;
}

BOOL CMyDialogBox::InitDialog(HWND hWnd)
{
	this->m_hwnd = hWnd;

	// show menubar
	ShowWindow(this->m_hwndMenu, SW_SHOW);

	// set the title bar
	SetWindowText(hWnd, this->m_szTitle);

	return this->InitComponents();
}