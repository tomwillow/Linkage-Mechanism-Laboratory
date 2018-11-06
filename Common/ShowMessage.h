#pragma once
#include <Windows.h>
#include "..\Common\tchar_head.h"

void ShowMessage(const TCHAR szFormat[], ...);
int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style, int iconid);
void ShowErrorMsgBox(LPTSTR lpszFunction, DWORD dwErrorCode);

bool IsErrorShowMsgBox(LPTSTR lpszFunction);