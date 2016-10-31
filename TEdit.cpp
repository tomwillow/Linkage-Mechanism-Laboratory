#include "TEdit.h"


TEdit::TEdit()
{
	m_hWnd = NULL;
	m_hInst = NULL;
}


TEdit::~TEdit()
{
}

WNDPROC TEdit::oldEditProc;
LRESULT CALLBACK TEdit::subEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TEdit * pEdit;
	pEdit=(TEdit *)GetWindowLong(hWnd, GWL_USERDATA);
	if (pEdit)
		return pEdit->WndProc(oldEditProc,hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc(oldEditProc, hWnd, uMsg, wParam, lParam);
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
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TEdit::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hParent,
	HMENU hMenu, HINSTANCE hInst)
{
	m_hInst = hInst;
	m_hWnd = CreateWindowEx(dwExStyle, lpszClass,
		lpszName, dwStyle, x, y, nWidth, nHeight,
		hParent, hMenu, hInst, 0);
}

void TEdit::CreateEditEx(HWND hParent,UINT id,HINSTANCE hInst)
{
	m_hInst = hInst;
	TEdit::CreateEx(WS_EX_CLIENTEDGE,
		TEXT("Edit"), 0,
		WS_CHILD,// | ES_AUTOHSCROLL | WS_VISIBLE
		0, 0, 0, 0, hParent,(HMENU)id, hInst);
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	oldEditProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)subEditProc);
}

void TEdit::SetPos(int x, int y, int width, int height)
{
	::SetWindowPos(m_hWnd, HWND_TOP, x,y,width,height,0);//SWP_SHOWWINDOW
}

void TEdit::ShowWindow(bool bShow)
{
	::ShowWindow(m_hWnd, bShow ? SW_SHOWNORMAL : SW_HIDE);
}

bool TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
	case VK_SPACE:
	case VK_RETURN:
		SetWindowText(this->m_hWnd, TEXT("no"));
		return false;
	}
	return true;
}

bool TEdit::OnChar(WPARAM wParam, LPARAM lParam)
{
	return false;
}

int TEdit::GetLength()
{
	return ::GetWindowTextLength(m_hWnd);
}

void TEdit::SetSelect(int iStart, int iEnd)
{
	SendMessage(m_hWnd, EM_SETSEL, iStart, iEnd);
}

void TEdit::SetSelectAll()
{
	SetSelect(0, GetLength());
}

void CDECL TEdit::SetText(TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	SetWindowText(m_hWnd, szBuffer);
}