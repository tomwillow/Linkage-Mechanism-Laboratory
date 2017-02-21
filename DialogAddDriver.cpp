#pragma once
#include <Windows.h>
#include <windowsx.h>

#include "resource.h"

BOOL CALLBACK DlgAddDriverProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND hComboDriverType = GetDlgItem(hDlg, IDC_COMBO_DRIVER_TYPE);
		ComboBox_AddString(hComboDriverType, TEXT("aa"));
		ComboBox_SetCurSel(hComboDriverType, 0);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}