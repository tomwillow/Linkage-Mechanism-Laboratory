#include "TTrackbar.h"


TTrackbar::TTrackbar()
{
}


TTrackbar::~TTrackbar()
{
}

void TTrackbar::CreateTrackbar(HWND hParent,HINSTANCE hInst,RECT rc,UINT id)
{   
	m_hWnd = CreateWindowEx(0, TRACKBAR_CLASS, 0,
TBS_AUTOTICKS | WS_CHILD | WS_VISIBLE,
rc.left, rc.top, rc.right, rc.bottom,
hParent, (HMENU)id, hInst, 0);

}

void TTrackbar::SetRange(int num)
{
// Set Trackbar range
SendMessage(m_hWnd, TBM_SETRANGE, 0, MAKELONG(0, num));
}

void TTrackbar::MoveWindow(RECT rect)
{
	::MoveWindow(m_hWnd, rect.left, rect.top, rect.right, rect.bottom, 0);
}

void TTrackbar::SetPos(int pos)
{
	SendMessage(m_hWnd, TBM_SETPOS, TRUE, pos);
}

int TTrackbar::GetPos()
{
	int pos;
	pos=SendMessage(m_hWnd, TBM_GETPOS, 0,0);
	return pos;
}