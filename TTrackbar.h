#pragma once
#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")

class TTrackbar
{
public:
	HWND m_hWnd;
	TTrackbar();
	~TTrackbar();
	void TTrackbar::CreateTrackbar(HWND hParent, HINSTANCE hInst, RECT rc, UINT id);
	void TTrackbar::SetRange(int num);
	void TTrackbar::MoveWindow(RECT rect);
	void TTrackbar::SetPos(int pos);
	int TTrackbar::GetPos();
};

