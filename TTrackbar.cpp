#pragma once
#include "TTrackbar.h"


TTrackbar::TTrackbar()
{
}


TTrackbar::~TTrackbar()
{
}

void TTrackbar::CreateTrackbar(HWND hParent, HINSTANCE hInst, RECT rc, UINT id)
{
	m_hParent = hParent;
	m_hInst = hInst;
	m_hWnd = CreateWindowEx(0, TRACKBAR_CLASS, 0,
		TBS_AUTOTICKS | WS_CHILD | WS_VISIBLE,
		rc.left, rc.top, rc.right, rc.bottom,
		hParent, (HMENU)id, hInst, 0);

	RegisterProc();
}

void TTrackbar::SetRange(int num)
{
	// Set Trackbar range
	SendMessage(m_hWnd, TBM_SETRANGE, 0, MAKELONG(0, num));
}

int TTrackbar::GetRange()
{
	return SendMessage(m_hWnd, TBM_GETRANGEMAX, 0, 0)-SendMessage(m_hWnd, TBM_GETRANGEMIN, 0, 0);
}

void TTrackbar::SetPos(int pos)
{
	SendMessage(m_hWnd, TBM_SETPOS, TRUE, pos);
}

int TTrackbar::GetPos()
{
	int pos;
	pos = SendMessage(m_hWnd, TBM_GETPOS, 0, 0);
	return pos;
}