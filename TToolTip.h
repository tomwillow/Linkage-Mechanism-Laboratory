#pragma once
#include <Windows.h>

class TToolTip
{
public:
	TToolTip();
	~TToolTip();
};

HWND CreateToolTip(HWND hParent, HWND hControl, HINSTANCE hInst, PTSTR pszText);