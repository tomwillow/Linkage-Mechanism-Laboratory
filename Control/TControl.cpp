#pragma once
#include "..\Common\DetectMemoryLeak.h"

#include "..\Common\tchar_head.h"
#include <stdio.h>//_vsnprintf_s
#include "TControl.h"

#include "..\Common\TTransfer.h"

TControl::TControl()
{
	m_hWnd = NULL;
	m_hParent = NULL;
	m_hInst = NULL;

	Text = NULL;

	m_hFont = NULL;
}


TControl::~TControl()
{
	if (Text != NULL)
		free(Text);

	if (m_hFont!=NULL)
	::DeleteObject(m_hFont);
}


void TControl::LinkControl(HWND hDlg, int id)
{
	LinkControl(GetDlgItem(hDlg, id));
}

//仅使用x,y坐标，width,height使用原大小
void TControl::SetPositionOnlyOrigin(const RECT &rect)
{
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	SetPosition(rect.left, rect.top, rc.right, rc.bottom);
}

//right和bottom保存的是宽和高
void TControl::SetRect(RECT &rect)
{
	::MoveWindow(m_hWnd, rect.left,rect.top,  rect.right, rect.bottom, true);
}

//对角点坐标
void TControl::SetRect(int x1, int y1, int x2, int y2)
{
	::MoveWindow(m_hWnd, x1, y1, x2 - x1, y2 - y1, true);
}


void TControl::SetPosition(int x, int y, int width, int height)
{
	::MoveWindow(m_hWnd, x, y, width, height, true);
	//::SetWindowPos(m_hWnd, HWND_TOP, x, y, width, height, 0);//SWP_SHOWWINDOW
}

//rect中各值均为坐标
void TControl::SetPosition(RECT rect)
{
	::MoveWindow(m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
	//::SetWindowPos(m_hWnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);//SWP_SHOWWINDOW
}

LRESULT CALLBACK TControl::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TControl * pControl;
#ifdef _WIN64
	pControl = (TControl *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
	pControl = (TControl *)GetWindowLong(hWnd, GWL_USERDATA);
#endif

	WNDPROC oldProc;
	oldProc = (WNDPROC)GetProp(hWnd, TEXT("oldProc"));
	if (pControl)
		return pControl->WndProc((WNDPROC)oldProc, hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc((WNDPROC)oldProc, hWnd, uMsg, wParam, lParam);
}

void TControl::LinkControl(HWND hwndControl)//链接到已有控件（用于对话框中）
{
	m_hWnd = hwndControl;
	RegisterProc();
}

void TControl::RegisterProc()//创建窗口后注册
{
#ifdef _WIN64
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
#else
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
#endif

	WNDPROC oldProc= (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)subControlProc);

	SetProp(m_hWnd, TEXT("oldProc"), oldProc);
}

LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//虚拟消息处理函数，可重载
{
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TControl::Invalidate()
{
	InvalidateRect(m_hWnd, &GetClientRect(), FALSE);
}

void TControl::SetFont(HFONT hFont)
{
	SendMessage(m_hWnd,             // Handle of edit control
		WM_SETFONT,         // Message to change the font
		(WPARAM)hFont,     // handle of the font
		MAKELPARAM(TRUE, 0) // Redraw text
		);
}

void TControl::SetDefaultGuiFont()
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	m_hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);//
	SetFont(m_hFont);
}

void TControl::SetFont(TCHAR FontName[], int FontSize)
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
	
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	_tcscpy_s(lf.lfFaceName,_tcslen(FontName)+1, FontName);
	lf.lfHeight = -MulDiv(FontSize, GetDeviceCaps(GetDC(m_hWnd), LOGPIXELSY), 72);
	//lf.lfWeight = 900;
	
	m_hFont = CreateFontIndirect(&lf);
	SetFont(m_hFont);
}

void TControl::SetText(const String &s)
{
	::SetWindowText(m_hWnd, s.c_str());
}

void CDECL TControl::SetText(const TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	::SetWindowText(m_hWnd, szBuffer);
}


void TControl::GetText(TCHAR text[])
{
	::GetWindowText(m_hWnd, text, GetLength() + 1);//不知道为什么要加1才取得全
}

TCHAR * TControl::GetText()
{
	//if (Text != NULL)
	//	free(Text);
	Text = (TCHAR *)realloc(Text, (GetLength() + 1)*sizeof(TCHAR));
	GetText(Text);
	return Text;
}

int TControl::GetLength()
{
	return ::GetWindowTextLength(m_hWnd);
}

void TControl::SetVisible(bool bShow)
{
	::ShowWindow(m_hWnd, bShow ? SW_SHOWNORMAL : SW_HIDE);
}

bool TControl::GetVisible()
{
	return (bool)IsWindowVisible(m_hWnd);
}

void TControl::SetEnable(bool bEnable)
{
	EnableWindow(m_hWnd, bEnable);
}

bool TControl::GetEnable()
{
	return (bool)IsWindowEnabled(m_hWnd);
}

//自动去掉小数末尾0，最多显示6位
void TControl::SetDouble(double d)
{
	TCHAR s[64];
	TTransfer::double2TCHAR_AutoTrim0(d, s);
	SetText(s);
}

double TControl::GetDouble()
{
	return _tcstod(GetText(), NULL);
}

//获得工具栏大小
RECT TControl::GetClientRect()
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	return rect;
}