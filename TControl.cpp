#include "TControl.h"


TControl::TControl()
{
}


TControl::~TControl()
{
}

void TControl::SetRect(RECT &rect)
{
	::MoveWindow(m_hWnd, rect.top, rect.left, rect.right, rect.bottom, true);
}

void TControl::SetRect(int x1,int y1,int x2,int y2)
{
	::MoveWindow(m_hWnd,x1,y1,x2-x1,y2-y1, true);
}