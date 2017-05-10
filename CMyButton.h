#pragma once
#include <Windows.h>

#include "TDraw.h"
#include "String.h"
#include "TControl.h"

class CMybutton:public TControl
{
private:
	HBITMAP m_BtBmp;
	HDC m_memBmp;
	HDC m_hDC;
	int m_BmpHeight;
	int m_BmpWidth;
	BOOL m_IsInRect;
	void CMybutton::DrawMyButton(HDC hdc, char state);
	LRESULT CMybutton::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void CMybutton::OnTimer(UINT nIDEvent);
	void CMybutton::OnMouseMove(UINT nFlags, POINT point);

public:
	CMybutton();
	virtual ~CMybutton();
	BOOL m_IsBmpLoad;
	virtual void CMybutton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void CMybutton::LoadBtBmp(UINT sID, int heigh, int width);


};