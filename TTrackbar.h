#pragma once
#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")

#include "TControl.h"

class TTrackbar:public TControl
{
public:
	TTrackbar();
	~TTrackbar();
	void TTrackbar::CreateTrackbar(HWND hParent, HINSTANCE hInst, RECT rc, UINT id);
	void TTrackbar::SetRange(int num);
	void TTrackbar::SetPos(int pos);
	int TTrackbar::GetPos();
};

