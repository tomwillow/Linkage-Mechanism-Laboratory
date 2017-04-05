#pragma once
#include "DialogOption.h"

#include <Windows.h>
#include <windowsx.h>

#include "resource.h"

#include "FileFunction.h"
#include "RegisterFunction.h"

#include "TButton.h"

#include "TMainWindow.h"
extern TMainWindow win;

BOOL CALLBACK DlgOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hCheckRegFile;
	static TButton CheckRegFile;
	static bool bHasRegFile;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		hCheckRegFile=GetDlgItem(hDlg, IDC_CHECK_REG_FILE);
		CheckRegFile.LinkControl(hCheckRegFile);

		TCHAR szSelfFileName[MAX_PATH];
		GetCommandLineByIndex(0, szSelfFileName);
		bHasRegFile=CheckFileAssociation(TEXT("mdsfile"), szSelfFileName);
		CheckRegFile.SetChecked(bHasRegFile);


		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_BUTTON_APPLY, 0), 0);
			EndDialog(hDlg, 0);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		case IDC_BUTTON_APPLY:
			if (CheckRegFile.GetChecked() != bHasRegFile)//更改了选项才操作
			{
				TCHAR szSelfFileName[MAX_PATH];
				GetCommandLineByIndex(0, szSelfFileName);
				long err_code;
				if (CheckRegFile.GetChecked())
					err_code = RegisterFileAssociation(TEXT(".mds"), TEXT("mdsfile"), szSelfFileName, szSelfFileName, IDI_ICON_FILE, TEXT("机构设计系统"));
				else
					err_code = UnRegisterFileAssociation(TEXT(".mds"), TEXT("mdsfile"));
				switch (err_code)
				{
				case 0:
					bHasRegFile = CheckRegFile.GetChecked();
					break;
				case 5:
					ShowMessage(TEXT("操作失败，请尝试以管理员身份运行程序。"));
					CheckRegFile.SetChecked(bHasRegFile);
					break;
				default:
					ShowMessage(TEXT("操作失败。错误码%d"), err_code);
					CheckRegFile.SetChecked(bHasRegFile);
					break;
				}
			}
			
			return TRUE;
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}