#pragma once

#include "DialogAbout.h"

#include <Windows.h>

INT_PTR CALLBACK DlgAboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, 0);
			return TRUE;
		}
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}