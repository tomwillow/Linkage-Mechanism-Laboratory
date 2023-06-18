#pragma once
#include "DialogOption.h"

#include <Windows.h>
#include <windowsx.h>

#include "..\resource.h"

#include "..\global.h"
#include "..\Common\FileFunction.h"
#include "..\Common\RegisterFunction.h"
#include "..\Common\ShowMessage.h"

#include "..\Control\TCheckBox.h"

#include "..\Control\TMainWindow.h"
extern TMainWindow win;

INT_PTR CALLBACK DlgOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCheckBox CheckRegFile;
	static TCheckBox CheckShowSchoolLogo;
	static TCheckBox CheckShowGrid;
	static bool bHasRegFile;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		//链接控件
		CheckRegFile.LinkControl(GetDlgItem(hDlg, IDC_CHECK_REG_FILE));

		//读取注册表状态
		TCHAR szSelfFileName[MAX_PATH];
		GetCommandLineByIndex(0, szSelfFileName);
		bHasRegFile=CheckFileAssociation(TEXT("lml files"), szSelfFileName);
		CheckRegFile.SetChecked(bHasRegFile);

		CheckShowSchoolLogo.LinkControl(GetDlgItem(hDlg, IDC_CHECK_SHOW_SCHOOL_LOGO));
		CheckShowSchoolLogo.SetChecked(win.m_Configuration.bDrawSchoolLogo);

		CheckShowGrid.LinkControl(GetDlgItem(hDlg, IDC_CHECK_SHOW_GRID));
		CheckShowGrid.SetChecked(win.m_Configuration.bDrawGrid);


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
					err_code = RegisterFileAssociation(TEXT(".lml"), TEXT("lml files"), szSelfFileName, szSelfFileName, IDI_ICON_FILE,szAppTitle);
				else
					err_code = UnRegisterFileAssociation(TEXT(".lml"), TEXT("lml files"));
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

			win.m_Configuration.bDrawSchoolLogo = CheckShowSchoolLogo.GetChecked();
			win.m_Configuration.bDrawGrid = CheckShowGrid.GetChecked();

			win.Canvas.Invalidate();
			
			return TRUE;
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}