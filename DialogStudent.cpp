#pragma once

#if (defined _STUDENT) || (defined _TEACHER)

#include "DialogStudent.h"

#include <Windows.h>
#include "String.h"

#include "resource.h"
#include "TEdit.h"
#include "ShowMessage.h"

#include "global.h"
namespace DialogStudent
{
	TEdit editStudentClass;
	TEdit editStudentName;
	TEdit editStudentNumber;
	BOOL CALLBACK DlgStudentProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_INITDIALOG:
		{
			editStudentClass.LinkControl(hDlg, IDC_EDIT_STU_CLASS);
			editStudentName.LinkControl(hDlg, IDC_EDIT_STU_NAME);
			editStudentNumber.LinkControl(hDlg, IDC_EDIT_STU_NUMBER);
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				sStudentClass = editStudentClass.GetText();
				if (sStudentClass.length() == 0)
				{
					ShowMessage(TEXT("班级不能为空！"));
					return TRUE;
				}
				sStudentName = editStudentName.GetText();
				if (sStudentName.length() == 0)
				{
					ShowMessage(TEXT("姓名不能为空！"));
					return TRUE;
				}
				sStudentNumber = editStudentNumber.GetText();
				if (sStudentNumber.length() == 0)
				{
					ShowMessage(TEXT("学号不能为空！"));
					return TRUE;
				}

				EndDialog(hDlg, 0);
				return TRUE;
			case IDCLOSE:
				EndDialog(hDlg, 0);
				ExitProcess(0);
				return TRUE;
			}
			return TRUE;
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			ExitProcess(0);
			return TRUE;
		}
		return FALSE;
	}
}

#endif