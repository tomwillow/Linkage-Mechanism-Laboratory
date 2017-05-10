#include "CMyButton.h"


//MyButton.cpp
CMybutton::CMybutton()
{
	m_IsInRect = FALSE;
	m_IsBmpLoad = FALSE;
	m_BmpHeight = 0;
	m_BmpWidth = 0;

}

CMybutton::~CMybutton()
{
}

void CMybutton::DrawMyButton(HDC hdc, char state)
{
	RECT buttonRect;
	GetClientRect(m_hWnd, &buttonRect);
	SetStretchBltMode(hdc, HALFTONE);
	LONG width = buttonRect.right - buttonRect.left;
	LONG height = buttonRect.bottom - buttonRect.top;
	if (m_IsBmpLoad == TRUE)
	{
		switch (state)
		{
		case 1:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, 0, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		case 2:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, m_BmpWidth, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		case 3:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, m_BmpWidth * 2, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		case 4:
			StretchBlt(hdc, 0, 0, width, height, m_memBmp, m_BmpWidth * 3, 0, m_BmpWidth
				, m_BmpHeight, SRCCOPY);
			break;
		default:
			break;
		}
	}
}

void CMybutton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your code to draw the specified item
	HDC dc;
	String buttonText;
	RECT buttonRect;
	UINT buttonState;
	if (m_IsBmpLoad == TRUE)
	{
		buttonState = lpDrawItemStruct->itemState;
		GetClientRect(m_hWnd, &buttonRect);

		TCHAR *sz = new TCHAR[GetWindowTextLength(m_hWnd) + 1];
		GetWindowText(m_hWnd, sz, GetWindowTextLength(m_hWnd));
		buttonText = sz;
		delete[] sz;

		if (buttonState & ODS_DISABLED)
		{
			DrawMyButton(m_hDC, 4);
			SetTextColor(m_hDC, RGB(80, 80, 80));
		}
		else if (buttonState & ODS_SELECTED)
		{
			DrawMyButton(m_hDC, 3);
		}
		else if (m_IsInRect == TRUE)
		{
			DrawMyButton(m_hDC, 2);
		}
		else 	//默认情况下
		{
			DrawMyButton(m_hDC, 1);
			SetTextColor(m_hDC, RGB(0, 0, 0));
		}
		if (buttonState & ODS_FOCUS)
		{
			RECT FocTect(buttonRect);	//构造焦点区域
			TDraw::SetMarginRect(&FocTect, 2);
			//FocTect.DeflateRect(2, 2, 2, 2);	//设置焦点区域大小
			DrawFocusRect(m_hDC, &FocTect);	//绘制焦点框
			lpDrawItemStruct->itemAction = ODA_FOCUS;
		}
		SetBkMode(m_hDC, TRANSPARENT);	//设置背景透明
		if (buttonState & ODS_SELECTED)
		{
			RECT rect;
			rect.left = buttonRect.left;
			rect.top = buttonRect.top + 2;
			rect.right = buttonRect.right + 2;
			rect.bottom = buttonRect.bottom;
			DrawText(m_hDC, buttonText.c_str(), buttonText.length(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	//绘制按钮文本
		}
		else
			DrawText(m_hDC, buttonText.c_str(), buttonText.length(), &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	//绘制按钮文本

	}
}


void CMybutton::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	POINT point;	//声明POINT变量
	GetCursorPos(&point); 	//获得鼠标位置
	RECT rcWnd;	//声明区域对象
	GetWindowRect(m_hWnd, &rcWnd); 	//获得按钮区域
	if (PtInRect(&rcWnd, point)) 	//判断鼠标是否在按钮上
	{


	}
	else	//鼠标移动到按钮上
	{
		m_IsInRect = FALSE;	//设置m_IsInRect变量值
		Invalidate(); 	  //重绘按钮
		KillTimer(m_hWnd, 1);
	}
}

void CMybutton::OnMouseMove(UINT nFlags, POINT point)
{
	// TODO: Add your message handler code here and/or call default
	RECT rcWnd;	//声明区域对象
	GetClientRect(m_hWnd, &rcWnd);
	if (PtInRect(&rcWnd, point))
	{
		if (m_IsInRect == FALSE)
		{
			m_IsInRect = TRUE;
			Invalidate();
			SetTimer(m_hWnd, 1, 50, NULL);
		}

	}
	//	CButton::OnMouseMove(nFlags, point);
}

void CMybutton::LoadBtBmp(UINT sID, int heigh, int width)
{
	m_hDC = GetWindowDC(m_hWnd);
	LoadBitmap(m_hInst, MAKEINTRESOURCE(sID));
	m_memBmp = CreateCompatibleDC(m_hDC);
	SelectObject(m_memBmp, m_BtBmp);
	m_BmpHeight = heigh;
	m_BmpWidth = width;
	m_IsBmpLoad = TRUE;
}


LRESULT CMybutton::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		OnTimer(lParam);
		return 0;
	case WM_MOUSEMOVE:
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		OnMouseMove(wParam, pt);
		return 0;
	case WM_DRAWITEM:
		DRAWITEMSTRUCT *DrawItemStruct = (DRAWITEMSTRUCT *)lParam;
		DrawItem(DrawItemStruct);
		return 0;
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}