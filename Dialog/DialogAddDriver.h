#pragma once
#include <Windows.h>

#include "../Control/TGraph.h"
#include "../Control/TEdit.h"


namespace DialogAddDriver
{
	extern int iElementId;
	extern double dElementValue;
	void UpdateDlgAddDriver();
	INT_PTR CALLBACK DlgAddDriverProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
}