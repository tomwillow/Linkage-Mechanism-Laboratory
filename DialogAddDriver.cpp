#pragma once
#include <Windows.h>
#include <windowsx.h>

#include "resource.h"

#include "TGraph.h"
#include "TEdit.h"

#include "TMainWindow.h"
extern TMainWindow win;

BOOL CALLBACK DlgAddDriverProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TEdit EditExpr;
	static TGraph *pGraph;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND hComboDriverType = GetDlgItem(hDlg, IDC_COMBO_DRIVER_TYPE);
		ComboBox_AddString(hComboDriverType, TEXT("位置"));
		ComboBox_AddString(hComboDriverType, TEXT("速度"));
		ComboBox_AddString(hComboDriverType, TEXT("加速度"));
		ComboBox_SetCurSel(hComboDriverType, 0);

		RECT rectEditExpr;
		GetWindowRect(GetDlgItem(hDlg, IDC_EDIT_FORMULAR), &rectEditExpr);

		EditExpr.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FORMULAR));
		EditExpr.SetText(TEXT("phi0=0.1*t"));

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

		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
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