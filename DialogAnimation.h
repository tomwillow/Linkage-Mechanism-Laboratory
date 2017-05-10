#pragma once
#include <Windows.h>

namespace DialogAnimation
{
	BOOL CALLBACK DlgAnimationProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void SetEditTimeAndEditFrame();
	void SetPlayerEnable(bool bEnable);
}