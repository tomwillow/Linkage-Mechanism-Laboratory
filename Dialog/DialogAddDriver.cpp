#pragma once
#include <Windows.h>
#include <windowsx.h>

#include "DialogAddDriver.h"

#include "..\resource.h"
#include "..\Element\TDriver.h"

#include "..\TConfiguration.h"
#include "..\Element\TShape.h"
#include "..\Control\TToolTip.h"
#include "..\Control\TTreeViewContent.h"
#include "..\Solver\TSolver.h"

#include "..\Common\TTransfer.h"
#include "..\Solver\TExpressionTree.h"
#include "..\Solver\TVariableTable.h"
#include "..\Common\String.h"

#include "..\Control\TMainWindow.h"
extern TMainWindow win;

namespace DialogAddDriver
{
	int iElementId;
	double dElementValue;
	TConfiguration *pConfig;
	TTreeViewContent *pTreeViewContent;
	TShape *pShape;
	TSolver *pSolver;

	TEdit EditA;
	TEdit EditB;
	TEdit LabelA;
	TEdit LabelB;
	TEdit EditExprRight;
	TEdit EditExprLeft;
	TEdit LabelDriverUnit;
	TCheckBox CheckBoxDisplacement;
	TCheckBox CheckBoxV;
	TCheckBox CheckBoxA;
	TGraph *pGraph;
	HWND hComboDriverType;
	HWND hComboExprType;
	HWND hToolTipError;

	//所有左侧控件更新进入此处
	void UpdateDlgAddDriver()
	{
		//根据类型选择单位
		switch (ComboBox_GetCurSel(hComboDriverType))
		{
		case 0://位移
			LabelDriverUnit.SetText(TEXT("rad"));
			break;
		case 1://速度
			LabelDriverUnit.SetText(TEXT("rad/sec"));
			break;
		case 2://加速度
			LabelDriverUnit.SetText(TEXT("rad/sec^2"));
			break;
		}

		//根据表达式类型决定显示A,B
		switch (ComboBox_GetCurSel(hComboExprType))
		{
		case 0://常量
			Edit_SetReadOnly(EditExprRight.m_hWnd, TRUE);
			EditA.SetVisible(true);
			LabelA.SetVisible(true);
			EditB.SetVisible(false);
			LabelB.SetVisible(false);
			switch (ComboBox_GetCurSel(hComboDriverType))
			{
			case 0://位移
				EditExprRight.SetText(TEXT("%f"), TTransfer::TCHAR2double(EditA.GetText()));
				break;
			case 1://速度
				EditExprRight.SetText(TEXT("%f*t"), TTransfer::TCHAR2double(EditA.GetText()));
				break;
			case 2://加速度
				EditExprRight.SetText(TEXT("0.5*%f*t^2"), TTransfer::TCHAR2double(EditA.GetText()));
				break;
			}
			break;
		case 1://直线
			Edit_SetReadOnly(EditExprRight.m_hWnd, TRUE);
			EditA.SetVisible(true);
			LabelA.SetVisible(true);
			EditB.SetVisible(true);
			LabelB.SetVisible(true);
			switch (ComboBox_GetCurSel(hComboDriverType))
			{
			case 0://位移
				EditExprRight.SetText(TEXT("%f*t+%f"), TTransfer::TCHAR2double(EditA.GetText()), TTransfer::TCHAR2double(EditB.GetText()));
				break;
			case 1://速度
				break;
			case 2://加速度
				break;
			}
			break;
		case 2://自定义
			Edit_SetReadOnly(EditExprRight.m_hWnd, FALSE);
			EditA.SetVisible(false);
			LabelA.SetVisible(false);
			EditB.SetVisible(false);
			LabelB.SetVisible(false);
			switch (ComboBox_GetCurSel(hComboDriverType))
			{
			case 0://位移
				break;
			case 1://速度
				break;
			case 2://加速度
				break;
			}
			break;

		}

		EditExprLeft.SetText(TEXT("phi%d"), iElementId);
	}

	INT_PTR CALLBACK DlgAddDriverProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{

		switch (message)
		{
		case WM_INITDIALOG:
		{
			pTreeViewContent = &(win.RightWindow.TreeViewContent);
			pShape = &(win.m_Shape);
			pConfig = &(win.m_Configuration);
			pSolver = win.pSolver;

			hToolTipError = NULL;

			RECT rcGraph;
			GetClientRect(GetDlgItem(hDlg, IDC_STATIC_GRAPH), &rcGraph);
			MapWindowRect(GetDlgItem(hDlg, IDC_STATIC_GRAPH), hDlg, &rcGraph);

			//
			pGraph = new TGraph(&(win.m_Configuration));
			pGraph->bShowMouseLine = true;
			pGraph->bShowLabelX = false;
			pGraph->bShowTitle = false;
			pGraph->CreateEx(0, TEXT("图表"), TEXT("图表"),
				WS_CHILD,
				300,
				50,
				300,
				200,
				hDlg, (HMENU)0,
#ifdef _WIN64
				(HINSTANCE)GetWindowLong(hDlg, GWLP_HINSTANCE)
#else
				(HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE)
#endif
			);
			pGraph->SetDoubleBuffer(true);
			pGraph->ShowWindow(SW_SHOWNORMAL);
			pGraph->SetWindowRect(rcGraph);
			pGraph->UpdateWindow();

			//
			hComboDriverType = GetDlgItem(hDlg, IDC_COMBO_DRIVER_TYPE);
			ComboBox_AddString(hComboDriverType, TEXT("位移"));
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
			EditExprRight.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FORMULAR_R));

			CheckBoxDisplacement.LinkControl(GetDlgItem(hDlg, IDC_CHECK_D));
			CheckBoxV.LinkControl(GetDlgItem(hDlg, IDC_CHECK_V));
			CheckBoxA.LinkControl(GetDlgItem(hDlg, IDC_CHECK_A));
			CheckBoxDisplacement.SetChecked(true);

			//初始设置部分
			//ComboBox_SetCurSel(hComboDriverType, 0);
			//ComboBox_SetCurSel(hComboExprType, 2);
			//EditExprRight.SetText(TEXT("sin(t)"));
			ComboBox_SetCurSel(hComboDriverType, 1);
			ComboBox_SetCurSel(hComboExprType, 0);
			EditA.SetText(TEXT("1"));
			//EditExprRight.SetText(TEXT("1"));

			UpdateDlgAddDriver();

			return TRUE;
		}

		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
		{
			HWND hwndControl = (HWND)lParam;
			if (hwndControl == EditExprRight.m_hWnd)
			{
				enumError err = (enumError)(int)GetProp(EditExprRight.m_hWnd, TEXT("error"));
				//OutputDebugInt(err);
				if (hToolTipError != NULL)
				{
					DestroyWindow(hToolTipError);
					hToolTipError = NULL;
				}
				if (err != ERROR_NO)//有错误
				{
					HDC hdc = (HDC)wParam;
					HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));

					SetBkMode(hdc, TRANSPARENT);

					//SetTextColor(hdc, RGB(255, 0, 0));

					TCHAR szErr[64];
					String sErr;
					sErr=GetErrorInfo(err);
					hToolTipError=CreateToolTip(hDlg, EditExprRight.m_hWnd,
#ifdef _WIN64
						(HINSTANCE)GetWindowLong(hDlg, GWLP_HINSTANCE)
#else
						(HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE)
#endif
						,szErr);

					return (LRESULT)hBrush;
				}
			}
			return 0;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{

				//改动Edit内容
			case IDC_EDIT_A:
			case IDC_EDIT_B:
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					UpdateDlgAddDriver();
				}
				break;
			}

			//Expr变动
			case IDC_EDIT_FORMULAR_R:
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					pGraph->Clear();
					try
					{

						TExpressionTree Expr, ExprV, ExprA;
						TExpressionTree ExprTemp, ExprTempV, ExprTempA;
						TVariableTable VariableTable;
						VariableTable.Define(NULL, TEXT("t"));
						String sz_t = VariableTable.VariableTable[0];
						Expr.LinkVariableTable(&VariableTable);
						ExprV.LinkVariableTable(&VariableTable);
						ExprA.LinkVariableTable(&VariableTable);
						ExprTemp.LinkVariableTable(&VariableTable);
						ExprTempV.LinkVariableTable(&VariableTable);
						ExprTempA.LinkVariableTable(&VariableTable);

						//读入Expr
						String s = EditExprRight.GetText();
						Expr.Read(s.c_str(), false);


						double value, valueV, valueA;

							std::vector<DPOINT> dptVector, dptVectorV, dptVectorA;
							ExprV = Expr;
							ExprV.Diff(sz_t, 1, false);
							ExprV.Simplify(false);

							ExprA = ExprV;
							ExprA.Diff(sz_t, 1, false);
							ExprA.Simplify(false);
							for (double t = 0; t <= 10; t += 0.1)
							{
								ExprTemp = Expr;
								ExprTemp.Subs(sz_t, t, false);
								value = ExprTemp.Value(true);

								ExprTempV = ExprV;
								ExprTempV.Subs(sz_t, t, false);
								valueV = ExprTempV.Value(true);

								ExprTempA = ExprA;
								ExprTempA.Subs(sz_t, t, false);
								valueA = ExprTempA.Value(true);
								//if ((err = Expr.GetError()) != ERROR_NO)
								//	break;
								dptVector.push_back({ t, value });
								dptVectorV.push_back({ t, valueV });
								dptVectorA.push_back({ t, valueA });
							}

							//循环结束，送入数据
								pGraph->InputDptVector(dptVector, { PS_SOLID, { 1, 0 }, 0 }, CheckBoxDisplacement.GetChecked(), TEXT("D"));
								pGraph->InputDptVector(dptVectorV, { PS_SOLID, { 1, 0 }, RGB(255, 0, 0) }, CheckBoxV.GetChecked(), TEXT("V"));
								pGraph->InputDptVector(dptVectorA, { PS_SOLID, { 1, 0 }, RGB(0, 255, 0) }, CheckBoxA.GetChecked(), TEXT("A"));
							
						//correct
					}
					catch (TError &err)
					{
						//存入错误码
						SetProp(EditExprRight.m_hWnd, TEXT("error"), (HANDLE)err.id);
					}
					//刷新
					//pGraph->Invalidate();

					//为更新Edit背景 刷新Dlg  将触发WM_CTLCOLOREDIT
					RECT ClientRect;
					GetClientRect(hDlg, &ClientRect);
					InvalidateRect(hDlg, &ClientRect, TRUE);
				}//EN_CHANGE
				break;
			}

			case IDC_CHECK_D:
				pGraph->SetPointDataVisible(0, CheckBoxDisplacement.GetChecked());
				pGraph->Invalidate();
				break;
			case IDC_CHECK_V:
				pGraph->SetPointDataVisible(1, CheckBoxV.GetChecked());
				pGraph->Invalidate();
				break;
			case IDC_CHECK_A:
				pGraph->SetPointDataVisible(2, CheckBoxA.GetChecked());
				pGraph->Invalidate();
				break;

				//改动ComboBox选项
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
			case IDC_BUTTON_SET_NOW_POS_IS_ORIGIN:
			{
				//计算t=0时刻的值，不等于当前位置则加上当前位置
				TExpressionTree Expr,ExprTemp;
				TVariableTable VariableTable;
				VariableTable.DefineOne(NULL, TEXT("t"), 0.0);
				Expr.LinkVariableTable(&VariableTable); 
				Expr.Read(EditExprRight.GetText(), false);
				Expr.OutputStr();

				ExprTemp = Expr;

				ExprTemp.Subs(TEXT("t"), 0.0, false);

				double value=0.0;
				try
				{
					ExprTemp.Calc(&value);
				}
				catch (enumError &err)
				{
					break;
				}

					if (!ISEQUAL(value,dElementValue,precision))
					{
						Expr.operator+(dElementValue);
						Expr.Simplify(false);

						EditExprRight.SetText(Expr.OutputStr());
					}
				break;
			}
			case IDOK:
			{
				//添加约束

				TDriver Driver;
				Driver.SetStyle(pConfig);

				Driver.sExprLeft = EditExprLeft.GetText();
				Driver.sExprRight = EditExprRight.GetText();

				pTreeViewContent->AddItem(&Driver, pShape->iNextId);
				pShape->AddElement(&Driver);
				pSolver->RefreshEquations();
			}
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