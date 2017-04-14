#pragma once
#include <Windows.h>

#include "TGraph.h"
#include "TEdit.h"


namespace DialogAddDriver
{
	extern int iElementId;
	void UpdateDlgAddDriver();
	BOOL CALLBACK DlgAddDriverProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
}