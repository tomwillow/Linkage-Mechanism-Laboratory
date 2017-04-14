#pragma once
#include <Windows.h>
#include <windowsx.h>

#include "DialogAddDriver.h"

#include "resource.h"

#include "TMainWindow.h"
extern TMainWindow win;

namespace DialogAddDriver
{
	int iElementId;
	TEdit EditA;
	TEdit EditB;
	TEdit LabelA;
	TEdit LabelB;
	TEdit EditExpr;
	TEdit EditExprLeft;
	TEdit LabelDriverUnit;
	TGraph *pGraph;
	HWND hComboDriverType;
	HWND hComboExprType;

	void UpdateDlgAddDriver()
	{
		switch (ComboBox_GetCurSel(hComboDriverType))
		{
		case 0:
			LabelDriverUnit.SetText(TEXT("deg"));
			break;
		case 1:
			LabelDriverUnit.SetText(TEXT("deg/sec"));
			break;
		case 2:
			LabelDriverUnit.SetText(TEXT("deg/sec^2"));
			break;

		}

		switch (ComboBox_GetCurSel(hComboExprType))
		{
		case 0:
			Edit_SetReadOnly(EditExpr.m_hWnd, TRUE);
			EditA.SetVisible(true);
			LabelA.SetVisible(true);
			EditB.SetVisible(false);
			LabelB.SetVisible(false);
			break;
		case 1:
			Edit_SetReadOnly(EditExpr.m_hWnd, TRUE);
			EditA.SetVisible(true);
			LabelA.SetVisible(true);
			EditB.SetVisible(true);
			LabelB.SetVisible(true);
			break;
		case 2:
			Edit_SetReadOnly(EditExpr.m_hWnd, FALSE);
			EditA.SetVisible(false);
			LabelA.SetVisible(false);
			EditB.SetVisible(false);
			LabelB.SetVisible(false);
			break;

		}

		EditExprLeft.SetText(TEXT("phi%d"),iElementId);
	}

	BOOL CALLBACK DlgAddDriverProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{

		switch (message)
		{
		case WM_INITDIALOG:
		{
			//
			hComboDriverType = GetDlgItem(hDlg, IDC_COMBO_DRIVER_TYPE);
			ComboBox_AddString(hComboDriverType, TEXT("位置"));
			ComboBox_AddString(hComboDriverType, TEXT("速度"));
			ComboBox_AddString(hComboDriverType, TEXT("加速度"));
			ComboBox_SetCurSel(hComboDriverType, 0);

			//
			hComboExprType = GetDlgItem(hDlg, IDC_COMBO_EXPR_TYPE);
			ComboBox_AddString(hComboExprType, TEXT("常量"));
			ComboBox_AddString(hComboExprType, TEXT("直线"));
			ComboBox_AddString(hComboExprType, TEXT("自定义"));
			ComboBox_SetCurSel(hComboExprType, 0);

			//
			EditA.LinkControl(GetDlgItem(hDlg, IDC_EDIT_A));
			EditB.LinkControl(GetDlgItem(hDlg, IDC_EDIT_B));

			LabelA.LinkControl(GetDlgItem(hDlg, IDC_STATIC_A));
			LabelB.LinkControl(GetDlgItem(hDlg, IDC_STATIC_B));

			//
			LabelDriverUnit.LinkControl(GetDlgItem(hDlg, IDC_STATIC_DRIVER_UNIT));


			EditExprLeft.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FORMULAR_L));

			////
			EditExpr.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FORMULAR_R));
			EditExpr.SetText(TEXT("0.1*t"));

			//
			pGraph = new TGraph(&(win.m_Configuration));
			pGraph->CreateEx(0, TEXT("图表"), TEXT("图表"),
				WS_CHILD,
				300,
				50,
				300,
				200,
				hDlg, (HMENU)0, (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE));
			pGraph->SetDoubleBuffer(true);
			pGraph->ShowWindow(SW_SHOWNORMAL);
			pGraph->UpdateWindow();

			std::vector<DPOINT> dptVector;
			DPOINT dpt;
			double y;
			for (double x = 0; x < 1000; ++x)
			{
				dpt.x = x;
				dpt.y = sin(x / 50);
				dptVector.push_back(dpt);
			}
			pGraph->InputDptVector(dptVector);


			UpdateDlgAddDriver();

			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_EDIT_FORMULAR_R:
			{

				if (HIWORD(wParam) == EN_CHANGE)
				{
					UpdateDlgAddDriver();
				}
				break;
			}
			case IDC_COMBO_EXPR_TYPE:
			case IDC_COMBO_DRIVER_TYPE:
			{
				WORD notification_code = HIWORD(wParam);
				if (notification_code == CBN_SELCHANGE)
				{
					UpdateDlgAddDriver();
				}
				break;
			}
			case IDOK:
				//添加约束

				//win.pSolver->AddMouseConstraint
			case IDCANCEL:
				EndDialog(hDlg, 0);
				return TRUE;
			}
			return TRUE;
		case WM_CLOSE:
			delete pGraph;
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return FALSE;
	}
}